// The GPU command-packet opcodes this title actually emits.
//
// Every row was read out of a writer instruction in the pinned `eboot.elf`; nothing here is copied
// from public PM4 documentation. See gnm_opcode_table.hpp for what is and is not grounded, and
// `tools/lift/check_gnm_opcode_table.py` for the byte-level re-derivation of each field.
#include "gnm_opcode_table.hpp"

namespace sotc::gnm {
namespace {

// opcode, name, family, size kind, min payload, max payload, stored-payload mask, writer sites,
// first RVA, last RVA, evidence at the anchor site.
constexpr OpcodeRow kRows[] = {
    {0x10, "inline_data_blob", Family::inline_data, SizeKind::unbounded,
     1, 63, 0x0ULL, 318, 0x8662C1, 0x885276,
     "0x86aa5e patches the count with shll 0x10 + orl; 0x86a238 stores 1 payload"},
    {0x13, "state_word_13", Family::state_word, SizeKind::fixed,
     1, 1, 0x1ULL, 1, 0x86BFE8, 0x86BFE8,
     "0x86bfe8: one payload dword, cursor advances 2 dwords"},
    {0x20, "state_word_20", Family::state_word, SizeKind::fixed,
     2, 2, 0x2ULL, 1, 0x86E7C5, 0x86E7C5,
     "0x86e7c5: two payload dwords unpacking one 64-bit argument"},
    {0x22, "address_value_pair", Family::addressed, SizeKind::fixed,
     4, 4, 0x8ULL, 3, 0x8663AF, 0x88334F,
     "0x8663af: 4 payload = 48-bit address word pair + 64-bit value pair"},
    {0x26, "state_word_26", Family::state_word, SizeKind::fixed,
     2, 2, 0x2ULL, 1, 0x86BF8F, 0x86BF8F,
     "0x86bf8f: two payload dwords, bit0 of the first masked off"},
    {0x2A, "state_word_2a", Family::state_word, SizeKind::fixed,
     1, 1, 0x1ULL, 1, 0x86BF1C, 0x86BF1C,
     "0x86bf1c: one packed state word built by and/or immediate field masks"},
    {0x2F, "state_word_2f", Family::state_word, SizeKind::fixed,
     1, 1, 0x1ULL, 1, 0x86C038, 0x86C038,
     "0x86c038: one payload dword"},
    {0x33, "address_mask_write", Family::addressed, SizeKind::fixed,
     3, 3, 0x4ULL, 2, 0x866648, 0x86668F,
     "0x866648: 3 payload, then mfence and sceGnmFlushGarlic on the same path"},
    {0x34, "state_block_34", Family::state_block, SizeKind::fixed,
     5, 5, 0x10ULL, 2, 0x86EFBF, 0x86F07C,
     "0x86efbf: five payload dwords packed from four arguments"},
    {0x37, "inline_data_37", Family::inline_data, SizeKind::unbounded,
     1, 4, 0x0ULL, 6, 0x86644D, 0x884BF1,
     "0x86644d patches the count; 0x884bf1 stores the 4-payload form"},
    {0x39, "state_word_39", Family::state_word, SizeKind::fixed,
     2, 2, 0x2ULL, 3, 0x86FB11, 0x8852EC,
     "0x86fb11: two payload dwords"},
    {0x3C, "descriptor_3c", Family::descriptor, SizeKind::fixed,
     6, 6, 0x20ULL, 7, 0x86D040, 0x884B42,
     "0x86d040: six payload dwords ending in the constant 0xa"},
    {0x3F, "state_block_3f", Family::state_block, SizeKind::fixed,
     3, 3, 0x4ULL, 2, 0x86FA48, 0x86FA8F,
     "0x86fa48: three payload dwords"},
    {0x46, "state_block_46", Family::state_block, SizeKind::closed,
     1, 3, 0x5ULL, 6, 0x86CF16, 0x88455E,
     "0x86cf16 stores one payload dword; 0x86e998 stores three"},
    {0x47, "state_block_47", Family::state_block, SizeKind::fixed,
     5, 5, 0x10ULL, 3, 0x86EBCD, 0x86ED8E,
     "0x86ebcd: five payload dwords from five arguments"},
    {0x50, "state_block_50", Family::state_block, SizeKind::fixed,
     6, 6, 0x20ULL, 24, 0x86A776, 0x8836B6,
     "0x86a776: six payload dwords packed from six arguments"},
    {0x58, "descriptor_58", Family::descriptor, SizeKind::fixed,
     6, 6, 0x20ULL, 4, 0x86CF9A, 0x884ECF,
     "0x86cf9a: six payload dwords ending in the constant 0xa"},
    {0x68, "register_write_2000", Family::register_write, SizeKind::unbounded,
     1, 2, 0x0ULL, 4, 0x86943A, 0x88322B,
     "0x86943a: payload[0] = argument - 0x2000, payload[1] = value"},
    {0x69, "register_write_a000", Family::register_write, SizeKind::unbounded,
     1, 15, 0x0ULL, 94, 0x86935A, 0x86FD9C,
     "0x86935a: payload[0] = argument - 0xA000, payload[1..] = values"},
    {0x76, "state_word_76", Family::state_word, SizeKind::unbounded,
     1, 2, 0x0ULL, 9, 0x8695F7, 0x86D660,
     "0x86966e: payload = constant index + value, 3-dword packets at 0xc stride"},
    {0x79, "register_write_4000", Family::register_write, SizeKind::unbounded,
     1, 4, 0x0ULL, 211, 0x86940C, 0x883C05,
     "0x86940c: payload[0] = argument - 0x4000, payload[1] = value"},
    {0x81, "inline_indexed_81", Family::inline_data, SizeKind::unbounded,
     1, 1, 0x0ULL, 2, 0x866113, 0x8661DB,
     "0x866113 patches the count, payload[1..] is memcpy'd index-like data"},
    {0x83, "state_block_83", Family::state_block, SizeKind::fixed,
     4, 4, 0x8ULL, 1, 0x86606A, 0x86606A,
     "0x86606a: four payload dwords, cursor advances 5 dwords"},
    {0x88, "word_argument_88", Family::state_word, SizeKind::fixed,
     1, 1, 0x1ULL, 1, 0x865FD8, 0x865FD8,
     "0x865fd8: one payload dword, cursor advances 2 dwords"},
    {0x8C, "inline_indexed_8c", Family::inline_data, SizeKind::unbounded,
     1, 5, 0x0ULL, 2, 0x86B7DD, 0x883B35,
     "0x86b7dd patches the count; 0x883b35 stores the base count"},
    {0x8D, "state_block_8d", Family::state_block, SizeKind::unbounded,
     3, 3, 0x0ULL, 1, 0x86B898, 0x86B898,
     "0x86b898: three payload dwords, count patched by orl"},
    {0x8E, "state_block_8e", Family::state_block, SizeKind::fixed,
     3, 3, 0x4ULL, 1, 0x86A8CA, 0x86A8CA,
     "0x86a8ca: three payload dwords, one field is an address shifted right 6"},
};

constexpr std::size_t kRowCount = sizeof(kRows) / sizeof(kRows[0]);

class TableSink final : public pm4::Sink {
public:
    explicit TableSink(ValidationSummary& summary) noexcept : summary_(summary) {}

    bool on_packet(const pm4::Packet& packet, const std::uint32_t*) override {
        if (packet.type != pm4::PacketType::command) {
            return true;
        }
        ++summary_.command_packets;
        HeaderInfo info{};
        const Verdict verdict = classify(packet.header, &info);
        if (verdict == Verdict::unknown_opcode) {
            ++summary_.unknown_opcodes_seen;
        }
        if (verdict != Verdict::ok) {
            ++summary_.rejected;
            summary_.verdict = verdict;
            summary_.bad_header_index = packet.header_index;
            return false;
        }
        ++summary_.known_opcodes;
        return true;
    }

private:
    ValidationSummary& summary_;
};

}  // namespace

const OpcodeRow* rows() noexcept { return kRows; }

std::size_t row_count() noexcept { return kRowCount; }

const OpcodeRow* find(std::uint16_t opcode) noexcept {
    if (opcode > 0xFFu) {
        return nullptr;
    }
    for (std::size_t i = 0; i < kRowCount; ++i) {
        if (kRows[i].opcode == opcode) {
            return &kRows[i];
        }
    }
    return nullptr;
}

const char* family_name(Family family) noexcept {
    switch (family) {
        case Family::register_write:
            return "register_write";
        case Family::inline_data:
            return "inline_data";
        case Family::addressed:
            return "addressed";
        case Family::descriptor:
            return "descriptor";
        case Family::state_word:
            return "state_word";
        case Family::state_block:
            return "state_block";
    }
    return "unknown";
}

const char* size_kind_name(SizeKind kind) noexcept {
    switch (kind) {
        case SizeKind::fixed:
            return "fixed";
        case SizeKind::closed:
            return "closed";
        case SizeKind::unbounded:
            return "unbounded";
    }
    return "unknown";
}

const char* verdict_name(Verdict verdict) noexcept {
    switch (verdict) {
        case Verdict::ok:
            return "ok";
        case Verdict::not_command_packet:
            return "not_command_packet";
        case Verdict::reserved_bits_set:
            return "reserved_bits_set";
        case Verdict::unknown_opcode:
            return "unknown_opcode";
        case Verdict::payload_size_mismatch:
            return "payload_size_mismatch";
    }
    return "unknown";
}

Verdict classify(std::uint32_t header, HeaderInfo* info) noexcept {
    const std::uint32_t payload = ((header >> 16) & 0x3FFFu) + 1u;
    const std::uint16_t opcode = static_cast<std::uint16_t>((header >> 8) & 0xFFu);
    if (info != nullptr) {
        info->opcode = static_cast<std::uint8_t>(opcode);
        info->payload_dwords = static_cast<std::uint16_t>(payload);
    }
    if ((header >> 30) != static_cast<std::uint32_t>(pm4::PacketType::command)) {
        return Verdict::not_command_packet;
    }
    // Bits [7:1] are reserved. Bit 0 is OR'd from a runtime flag at writer 0x86b7dd, so it is not
    // treated as malformed; its GPU meaning is not grounded.
    if ((header & 0xFEu) != 0) {
        return Verdict::reserved_bits_set;
    }
    const OpcodeRow* row = find(opcode);
    if (row == nullptr) {
        return Verdict::unknown_opcode;
    }
    if (row->size_kind == SizeKind::unbounded) {
        return (payload >= row->min_payload) ? Verdict::ok : Verdict::payload_size_mismatch;
    }
    // A stored-size row: the mask holds exactly the payloads the writers were seen to emit.
    if (payload > 64u) {
        return Verdict::payload_size_mismatch;
    }
    const std::uint64_t bit = std::uint64_t{1} << (payload - 1u);
    return ((row->payload_mask & bit) != 0) ? Verdict::ok : Verdict::payload_size_mismatch;
}

ValidationSummary validate(const std::uint32_t* words, std::size_t word_count,
                           pm4::Limits limits) noexcept {
    ValidationSummary summary{};
    TableSink sink(summary);
    const pm4::Result result = pm4::decode(words, word_count, sink, limits);
    summary.decode = result.status;
    if (result.status != pm4::Status::ok && summary.rejected == 0) {
        // Framing failed before any verdict was reached: report the offset the decoder stopped at.
        summary.bad_header_index = result.stop_index;
    }
    return summary;
}

}  // namespace sotc::gnm
