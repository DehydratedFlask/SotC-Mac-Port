#include "pm4_decoder.hpp"

namespace sotc::pm4 {
namespace {

constexpr std::uint32_t kMaxPayloadDwords = 0x4000;  // [29:16] is a 14-bit count-minus-one field
constexpr std::uint8_t kTypeShift = 30;
constexpr std::uint32_t kCountShift = 16;
constexpr std::uint32_t kCountMask = 0x3FFFu;

std::uint32_t packet_type(std::uint32_t header) noexcept {
    return header >> kTypeShift;
}

std::uint32_t payload_words(std::uint32_t header) noexcept {
    return ((header >> kCountShift) & kCountMask) + 1u;
}

class CountingSink final : public Sink {
public:
    bool on_packet(const Packet&, const std::uint32_t*) override { return true; }
};

}  // namespace

const char* status_name(Status status) noexcept {
    switch (status) {
        case Status::ok:
            return "ok";
        case Status::empty_buffer:
            return "empty_buffer";
        case Status::truncated_header:
            return "truncated_header";
        case Status::truncated_payload:
            return "truncated_payload";
        case Status::oversized_packet:
            return "oversized_packet";
        case Status::reserved_bits_set:
            return "reserved_bits_set";
        case Status::too_many_packets:
            return "too_many_packets";
        case Status::sink_rejected:
            return "sink_rejected";
    }
    return "unknown";
}

Result decode(const std::uint32_t* words, std::size_t word_count, Sink& sink,
              Limits limits) noexcept {
    Result result{};
    if (words == nullptr || word_count == 0) {
        result.status = Status::empty_buffer;
        result.stop_index = 0;
        return result;
    }

    const std::uint32_t max_payload =
        (limits.max_payload_dwords == 0) ? kMaxPayloadDwords : limits.max_payload_dwords;

    std::size_t index = 0;
    while (index < word_count) {
        if (limits.max_packets != 0 && result.stats.packets >= limits.max_packets) {
            result.status = Status::too_many_packets;
            result.stop_index = index;
            return result;
        }

        const std::uint32_t header = words[index];
        const std::uint32_t type = packet_type(header);

        if (type == static_cast<std::uint32_t>(PacketType::nop)) {
            if (((header >> kCountShift) & kCountMask) != 0) {
                result.status = Status::reserved_bits_set;
                result.stop_index = index;
                return result;
            }
            Packet packet{};
            packet.type = PacketType::nop;
            packet.header = header;
            packet.header_index = index;
            packet.payload_index = index + 1;
            ++result.stats.nops;
            ++result.stats.packets;
            if (!sink.on_packet(packet, nullptr)) {
                result.status = Status::sink_rejected;
                result.stop_index = index;
                return result;
            }
            index += 1;
            continue;
        }

        const std::uint32_t count = payload_words(header);
        if (count > max_payload) {
            result.status = Status::oversized_packet;
            result.stop_index = index;
            return result;
        }
        if (index + 1 >= word_count) {
            result.status = Status::truncated_header;
            result.stop_index = index;
            return result;
        }
        if (count > word_count - (index + 1)) {
            result.status = Status::truncated_payload;
            result.stop_index = index;
            return result;
        }

        Packet packet{};
        packet.header = header;
        packet.count = static_cast<std::uint16_t>(count);
        packet.header_index = index;
        packet.payload_index = index + 1;

        switch (type) {
            case static_cast<std::uint32_t>(PacketType::register_write):
                packet.type = PacketType::register_write;
                packet.reg_index = static_cast<std::uint16_t>(header & 0xFFFFu);
                ++result.stats.register_writes;
                break;
            case static_cast<std::uint32_t>(PacketType::filler):
                packet.type = PacketType::filler;
                ++result.stats.fillers;
                break;
            case static_cast<std::uint32_t>(PacketType::command):
                packet.type = PacketType::command;
                packet.opcode = static_cast<std::uint16_t>((header >> 8) & 0xFFu);
                ++result.stats.commands;
                result.stats.command_dwords += count;
                break;
            default:
                result.status = Status::reserved_bits_set;
                result.stop_index = index;
                return result;
        }

        result.stats.payload_dwords += count;
        ++result.stats.packets;
        if (!sink.on_packet(packet, words + index + 1)) {
            result.status = Status::sink_rejected;
            result.stop_index = index;
            return result;
        }
        index += 1 + count;
    }

    result.status = Status::ok;
    result.stop_index = index;
    return result;
}

Result count_packets(const std::uint32_t* words, std::size_t word_count, Limits limits) noexcept {
    CountingSink sink;
    return decode(words, word_count, sink, limits);
}

}  // namespace sotc::pm4
