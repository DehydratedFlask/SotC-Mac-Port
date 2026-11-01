#include "../platform/pm4_decoder.hpp"

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string_view>
#include <vector>

namespace {

using sotc::pm4::Limits;
using sotc::pm4::Packet;
using sotc::pm4::PacketType;
using sotc::pm4::Sink;
using sotc::pm4::Status;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct RecordedPacket {
    PacketType type = PacketType::nop;
    std::uint16_t opcode = 0;
    std::uint16_t reg_index = 0;
    std::uint16_t count = 0;
    std::size_t header_index = 0;
    std::uint32_t payload0 = 0;
};

class RecordingSink final : public Sink {
public:
    std::vector<RecordedPacket> packets;
    std::size_t reject_at = static_cast<std::size_t>(-1);

    bool on_packet(const Packet& packet, const std::uint32_t* payload) override {
        RecordedPacket rec{};
        rec.type = packet.type;
        rec.opcode = packet.opcode;
        rec.reg_index = packet.reg_index;
        rec.count = packet.count;
        rec.header_index = packet.header_index;
        if (payload != nullptr && packet.count > 0) {
            rec.payload0 = payload[0];
        }
        packets.push_back(rec);
        return packets.size() - 1 != reject_at;
    }
};

// type 3, opcode, payload dwords
constexpr std::uint32_t command(std::uint16_t opcode, std::uint32_t payload_dwords) {
    return (3u << 30) | (((payload_dwords - 1u) & 0x3FFFu) << 16) |
           ((static_cast<std::uint32_t>(opcode) & 0xFFu) << 8);
}

// type 1, register byte offset, payload dwords
constexpr std::uint32_t register_write(std::uint16_t offset, std::uint32_t payload_dwords) {
    return (1u << 30) | (((payload_dwords - 1u) & 0x3FFFu) << 16) | offset;
}

constexpr std::uint32_t filler(std::uint32_t payload_dwords) {
    return (2u << 30) | (((payload_dwords - 1u) & 0x3FFFu) << 16);
}

constexpr std::uint32_t nop() { return 0u; }

void test_empty_buffer() {
    RecordingSink sink;
    const auto result = sotc::pm4::decode(nullptr, 0, sink);
    check("empty status", result.status == Status::empty_buffer);
    check("empty no packets", sink.packets.empty());
}

void test_nop_only() {
    const std::uint32_t words[] = {nop(), nop()};
    RecordingSink sink;
    const auto result = sotc::pm4::decode(words, 2, sink);
    check("nop ok", result.status == Status::ok);
    check("nop counted", result.stats.nops == 2 && result.stats.packets == 2);
    check("nop stop", result.stop_index == 2);
    check("nop payload none", sink.packets.size() == 2 && sink.packets[0].count == 0);
}

void test_register_write_single() {
    const std::uint32_t words[] = {register_write(0x1234, 1), 0xDEADBEEF};
    RecordingSink sink;
    const auto result = sotc::pm4::decode(words, 2, sink);
    check("reg ok", result.status == Status::ok);
    check("reg type", sink.packets.size() == 1 && sink.packets[0].type == PacketType::register_write);
    check("reg offset", sink.packets[0].reg_index == 0x1234);
    check("reg count", sink.packets[0].count == 1);
    check("reg payload", sink.packets[0].payload0 == 0xDEADBEEF);
    check("reg stats", result.stats.register_writes == 1 && result.stats.payload_dwords == 1);
}

void test_register_write_multi() {
    const std::uint32_t words[] = {register_write(0x0004, 3), 1, 2, 3};
    RecordingSink sink;
    const auto result = sotc::pm4::decode(words, 4, sink);
    check("multi ok", result.status == Status::ok);
    check("multi count", sink.packets.size() == 1 && sink.packets[0].count == 3);
    check("multi payload", sink.packets[0].payload0 == 1);
    check("multi stop", result.stop_index == 4);
}

void test_command_packet() {
    const std::uint32_t words[] = {command(0x2A, 2), 0xCAFEBABE, 0x12345678, nop()};
    RecordingSink sink;
    const auto result = sotc::pm4::decode(words, 4, sink);
    check("cmd ok", result.status == Status::ok);
    check("cmd type", sink.packets[0].type == PacketType::command);
    check("cmd opcode", sink.packets[0].opcode == 0x2A);
    check("cmd count", sink.packets[0].count == 2);
    check("cmd dwords", result.stats.command_dwords == 2);
    check("cmd trailing nop", result.stats.nops == 1);
}

void test_filler_skipped() {
    const std::uint32_t words[] = {filler(2), 0, 0, nop()};
    RecordingSink sink;
    const auto result = sotc::pm4::decode(words, 4, sink);
    check("filler ok", result.status == Status::ok);
    check("filler type", sink.packets[0].type == PacketType::filler);
    check("filler count", sink.packets[0].count == 2);
    check("filler clock", result.stats.fillers == 1);
}

void test_truncated_payload() {
    const std::uint32_t words[] = {command(0x10, 4), 1, 2};
    RecordingSink sink;
    const auto result = sotc::pm4::decode(words, 3, sink);
    check("truncated status", result.status == Status::truncated_payload);
    check("truncated stop", result.stop_index == 0);
    check("truncated no sink call", sink.packets.empty());
}

void test_truncated_header() {
    const std::uint32_t words[] = {register_write(0x10, 1)};
    RecordingSink sink;
    const auto result = sotc::pm4::decode(words, 1, sink);
    check("header status", result.status == Status::truncated_header);
}

void test_nop_reserved_bits() {
    const std::uint32_t words[] = {0x00010000u};  // type 0 with a non-zero [29:16] field
    RecordingSink sink;
    const auto result = sotc::pm4::decode(words, 1, sink);
    check("nop reserved", result.status == Status::reserved_bits_set);
}

void test_oversized_packet() {
    const std::uint32_t words[] = {command(0x20, 4), 1, 2, 3, 4};
    RecordingSink sink;
    Limits limits{};
    limits.max_payload_dwords = 2;
    const auto result = sotc::pm4::decode(words, 5, sink, limits);
    check("oversized", result.status == Status::oversized_packet);
}

void test_packet_cap() {
    const std::uint32_t words[] = {nop(), nop(), nop()};
    RecordingSink sink;
    Limits limits{};
    limits.max_packets = 2;
    const auto result = sotc::pm4::decode(words, 3, sink, limits);
    check("cap status", result.status == Status::too_many_packets);
    check("cap count", sink.packets.size() == 2);
}

void test_sink_reject() {
    const std::uint32_t words[] = {nop(), nop(), nop()};
    RecordingSink sink;
    sink.reject_at = 1;
    const auto result = sotc::pm4::decode(words, 3, sink);
    check("reject status", result.status == Status::sink_rejected);
    check("reject count", sink.packets.size() == 2);
}

void test_mixed_stream_census() {
    // A stream shaped like a real submission: state, constants, a draw-like command, a filler.
    const std::uint32_t words[] = {register_write(0x0000, 2), 0x11111111, 0x22222222,
                                   command(0x2D, 1), 0x00000001,
                                   register_write(0x0100, 1), 0x33333333,
                                   filler(1), 0x44444444,
                                   command(0x34, 0x10), 1, 2, 3, 4, 5, 6, 7, 8,
                                   9, 10, 11, 12, 13, 14, 15, 16,
                                   nop()};
    RecordingSink sink;
    const auto result = sotc::pm4::decode(words, 27, sink);
    check("mixed ok", result.status == Status::ok);
    check("mixed packets", result.stats.packets == 6);
    check("mixed regs", result.stats.register_writes == 2);
    check("mixed cmds", result.stats.commands == 2);
    check("mixed filler", result.stats.fillers == 1);
    check("mixed nop", result.stats.nops == 1);
    check("mixed payload", result.stats.payload_dwords == 2 + 1 + 1 + 1 + 16);
    check("mixed stop", result.stop_index == 27);
    check("mixed last header", sink.packets.back().header_index == 26);
}

void test_count_packets_helper() {
    const std::uint32_t words[] = {nop(), register_write(0x20, 1), 7};
    const auto result = sotc::pm4::count_packets(words, 3);
    check("count ok", result.status == Status::ok);
    check("count packets", result.stats.packets == 2);
    check("count regs", result.stats.register_writes == 1);
}

void test_status_names() {
    check("name ok", std::string_view(sotc::pm4::status_name(Status::ok)) == "ok");
    check("name truncated", std::string_view(sotc::pm4::status_name(Status::truncated_payload)) ==
                                "truncated_payload");
    check("name unknown", std::string_view(sotc::pm4::status_name(static_cast<Status>(200))) ==
                              "unknown");
}

// `--census <hexfile>` frames a stream supplied by tools/lift/check_pm4_decoder.py and prints a
// canonical one-line census, so an independent Python framing can be compared against this one.
int census_mode(const char* path) {
    std::ifstream input(path);
    if (!input) {
        std::fprintf(stderr, "cannot read %s\n", path);
        return 2;
    }
    std::vector<std::uint32_t> words;
    std::string token;
    while (input >> token) {
        words.push_back(static_cast<std::uint32_t>(std::strtoul(token.c_str(), nullptr, 16)));
    }
    const auto result = sotc::pm4::count_packets(words.data(), words.size());
    std::printf("%u %u %u %u %u %u %u %s %zu\n", result.stats.packets,
                result.stats.register_writes, result.stats.commands, result.stats.fillers,
                result.stats.nops, result.stats.payload_dwords, result.stats.command_dwords,
                sotc::pm4::status_name(result.status), result.stop_index);
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc == 3 && std::string_view(argv[1]) == "--census") {
        return census_mode(argv[2]);
    }
    test_empty_buffer();
    test_nop_only();
    test_register_write_single();
    test_register_write_multi();
    test_command_packet();
    test_filler_skipped();
    test_truncated_payload();
    test_truncated_header();
    test_nop_reserved_bits();
    test_oversized_packet();
    test_packet_cap();
    test_sink_reject();
    test_mixed_stream_census();
    test_count_packets_helper();
    test_status_names();

    if (failures == 0) {
        std::printf("OK test_pm4_decoder\n");
        return 0;
    }
    std::fprintf(stderr, "test_pm4_decoder: %d failure(s)\n", failures);
    return 1;
}
