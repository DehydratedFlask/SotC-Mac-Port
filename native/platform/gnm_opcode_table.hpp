// The GPU command-packet opcodes this title actually emits.
//
// The game does not hand its command stream to `sceGnmSubmitCommandBuffers` as opaque driver output
// only: the pinned executable contains its own packet writers, which build type-3 PM4 headers in a
// growable buffer. Every row here was read out of those writer instructions; nothing is copied from
// public PM4 tables and no register, draw or state semantics are invented.
//
// What is grounded (byte-bound, per writer site):
//   * The header is `0xC0000000 | (opcode << 8) | ((payload_dwords - 1) << 16)`.
//     The writers either store that dword complete (`movl $imm32, disp(reg)`) or build the count at
//     runtime with `shll $0x10` + `orl $0xC0..` (an `orl $imm32, reg`), then advance the buffer
//     cursor by exactly `1 + payload_dwords` dwords.
//   * Bits [7:1] of a header are always zero at every stored site. Bit 0 is not reserved: the writer
//     at `0x86b7dd` ORs `*(this + 0x28) & 1` into it (`andl $0x1` then `orl $0xC0048C00`), and the
//     same function then writes five payload dwords and advances the cursor by six, so the packet is
//     real. Only bit 0 is ever observed; [7:1] stay rejected.
//   * Which opcode values appear, how many writer sites emit each, and the payload sizes those
//     sites produce. `payload_dwords == count + 1` is confirmed at sites where the same basic block
//     also advances the cursor: `0x865fd8` stores count 0 and writes 2 dwords, `0x86606a` stores
//     count 3 and writes 5, `0x869b1f` stores count 4 and writes 6, `0x86a776` stores count 5 and
//     writes 7. `0x86aa5e` shows the runtime form (`shll $0x10` then `orl $0xC0001000`) whose count
//     the caller computes from an index length.
//
// What is NOT grounded, and is therefore absent:
//   * The GPU-side meaning of each opcode. No stream was captured on hardware, so a row carries a
//     structural `Family` (where the payload fields are) and never a claim such as "this is a draw".
//   * Opcode 0x00. `0xC0000000` is a ubiquitous constant in this image (274+ immediates used as
//     masks/flags) and no store of it exists in the emitter region, so it stays unknown rather than
//     being admitted on a coincidence.
//   * Any opcode with no writer site in this executable. Streams that use them are rejected.
//
// Regeneration/drift check: `python3 tools/lift/check_gnm_opcode_table.py` re-derives every row from
// the pinned `local/re/eboot.elf` and fails on any difference.
#pragma once

#include "pm4_decoder.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc::gnm {

// Where the payload fields sit. Structural only: derived from how the writer fills them.
enum class Family : std::uint8_t {
    register_write,  // payload[0] is a register index (the writer subtracts a space bias), rest values
    inline_data,     // payload[0] is a constant selector dword, payload[1..] is copied data
    addressed,       // payload holds a 48/64-bit address plus a value word
    descriptor,      // fixed multi-word block that ends in a constant selector/count
    state_word,      // one or two payload words of packed state, no address and no index
    state_block,     // fixed payload larger than two words, field roles not established
};

// How strictly a header's payload count is pinned by the evidence.
enum class SizeKind : std::uint8_t {
    fixed,      // exactly one payload size; every writer stores that same count
    closed,     // a closed set of stored payload sizes, recorded bit-for-bit in `payload_mask`
    unbounded,  // at least `min_payload`; a writer builds the count at runtime, so > max is legal
};

struct OpcodeRow {
    std::uint8_t opcode;
    const char* name;
    Family family;
    SizeKind size_kind;
    std::uint16_t min_payload;   // payload dwords, excluding the header
    std::uint16_t max_payload;   // == min when fixed; largest observed otherwise
    // Bit (payload - 1) set for every payload size a writer actually stores. Unused (0) for
    // `unbounded` rows, where the count is patched from a runtime value and only the floor is pinned.
    std::uint64_t payload_mask;
    std::uint32_t writer_sites;  // number of grounded writer instructions in the pinned image
    std::uint32_t first_site_rva;  // lowest writer RVA (image-relative, no bias)
    std::uint32_t last_site_rva;
    const char* evidence;  // what the bytes show at the anchor site
};

[[nodiscard]] const OpcodeRow* rows() noexcept;
[[nodiscard]] std::size_t row_count() noexcept;
[[nodiscard]] const OpcodeRow* find(std::uint16_t opcode) noexcept;

const char* family_name(Family family) noexcept;
const char* size_kind_name(SizeKind kind) noexcept;

enum class Verdict : std::uint8_t {
    ok = 0,
    not_command_packet,    // packet type is not 3; the decoder frames it, this table has no opinion
    reserved_bits_set,     // bits[7:1] of a type-3 header are non-zero
    unknown_opcode,        // no writer site in the pinned image emits this opcode
    payload_size_mismatch  // payload count is not one this opcode's writers produce
};

const char* verdict_name(Verdict verdict) noexcept;

struct HeaderInfo {
    std::uint8_t opcode = 0;
    std::uint16_t payload_dwords = 0;
};

// Validates one packet header against the table. `info` may be null.
[[nodiscard]] Verdict classify(std::uint32_t header, HeaderInfo* info = nullptr) noexcept;

struct ValidationSummary {
    pm4::Status decode = pm4::Status::ok;  // result of the framing pass
    Verdict verdict = Verdict::ok;         // first verdict that was not ok
    std::size_t bad_header_index = 0;      // dword index of that header
    std::uint32_t command_packets = 0;
    std::uint32_t known_opcodes = 0;
    std::uint32_t rejected = 0;
    std::uint32_t unknown_opcodes_seen = 0;
};

// Frames `word_count` dwords with sotc::pm4::decode and validates every type-3 packet against the
// table. Stops at the first rejected packet, so `bad_header_index` points at the offending header.
[[nodiscard]] ValidationSummary validate(const std::uint32_t* words, std::size_t word_count,
                                        pm4::Limits limits = {}) noexcept;

}  // namespace sotc::gnm
