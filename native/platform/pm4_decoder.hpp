// PS4 GNM / PM4 packet framing.
//
// The game imports `libSceGnmDriver` (101 symbols) and `libSceVideoOut` (24 symbols) — see
// `local/re/imports.json` — so its rendering path is a PM4 command stream submitted through
// `sceGnmSubmitCommandBuffers`. This module frames that stream; it does not interpret draws,
// register semantics or resource descriptors yet.
//
// Framing rules (public AMD PM4 packet layout, 32-bit little-endian dwords):
//   bits [31:30] = packet type
//     type 0 (nop)      : exactly one dword. Bits [29:16] are reserved here and must be 0;
//                         a non-zero value is rejected instead of guessed.
//     type 1 (register) : [15:0] = register base index, [29:16] = payload dwords minus one.
//     type 2 (filler)   : [29:16] = skipped dwords minus one; payload is not interpreted.
//     type 3 (command)  : [15:8] = opcode, [29:16] = payload dwords minus one.
//
// The two field positions above (type-3 opcode in bits 15:8, type-1 base index in bits 15:0) are
// the public PM4 header layout; the *scaling* of the type-1 field (dwords vs bytes) and the
// meaning of any particular opcode are NOT claimed here. They are added only when byte-bound
// evidence for this game exists.
//
// Every packet is bounds-checked against the buffer before the sink sees it, so a truncated or
// over-long stream stops with an explicit error instead of reading past the end. Opcode and
// register meanings are deliberately not encoded here: they are added only when byte-bound
// evidence for this game exists.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc::pm4 {

enum class PacketType : std::uint8_t { nop = 0, register_write = 1, filler = 2, command = 3 };

enum class Status : std::uint8_t {
    ok = 0,
    empty_buffer,
    truncated_header,
    truncated_payload,
    oversized_packet,
    reserved_bits_set,
    too_many_packets,
    sink_rejected,
};

const char* status_name(Status status) noexcept;

struct Packet {
    PacketType type = PacketType::nop;
    std::uint32_t header = 0;
    std::uint16_t opcode = 0;     // type 3 only, raw bits [15:8]
    std::uint16_t reg_index = 0;  // type 1 only, raw bits [15:0]
    std::uint16_t count = 0;       // payload dwords (0 for nop)
    std::size_t header_index = 0;  // index of the header dword in the buffer
    std::size_t payload_index = 0; // index of the first payload dword
};

struct Stats {
    std::uint32_t packets = 0;
    std::uint32_t nops = 0;
    std::uint32_t register_writes = 0;
    std::uint32_t fillers = 0;
    std::uint32_t commands = 0;
    std::uint32_t payload_dwords = 0;
    std::uint32_t command_dwords = 0; // payload dwords carried by type-3 packets
};

// Return false to stop decoding. The decoder then reports Status::sink_rejected.
class Sink {
public:
    virtual ~Sink() = default;
    [[nodiscard]] virtual bool on_packet(const Packet& packet, const std::uint32_t* payload) = 0;
};

struct Limits {
    // 0 means "use the packet-format maximum" (0x4000 dwords per packet).
    std::uint32_t max_payload_dwords = 0;
    // 0 means unlimited.
    std::uint32_t max_packets = 0;
};

struct Result {
    Status status = Status::ok;
    Stats stats{};
    std::size_t stop_index = 0; // dword index where decoding stopped (buffer end on success)
};

// Frames `word_count` dwords starting at `words`. `words` may be null only when `word_count` is 0.
[[nodiscard]] Result decode(const std::uint32_t* words, std::size_t word_count, Sink& sink,
                            Limits limits = {}) noexcept;

// Convenience: frame the buffer with a sink that accepts everything and return only the summary.
[[nodiscard]] Result count_packets(const std::uint32_t* words, std::size_t word_count,
                                   Limits limits = {}) noexcept;

}  // namespace sotc::pm4
