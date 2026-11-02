// Fixture for native/platform/gnm_opcode_table.*
//
// Every case is synthetic: headers are built here from the documented type-3 layout, never read from
// a captured game stream. The rows themselves are grounded in the pinned image, and
// `tools/lift/check_gnm_opcode_table.py` re-derives them from those bytes; this file checks the
// decoder contract that sits on top of the table.
#include "../platform/gnm_opcode_table.hpp"

#include <cstdint>
#include <cstdio>
#include <string_view>
#include <vector>

namespace {

using sotc::gnm::Family;
using sotc::gnm::HeaderInfo;
using sotc::gnm::OpcodeRow;
using sotc::gnm::SizeKind;
using sotc::gnm::Verdict;
using sotc::pm4::Limits;
using sotc::pm4::PacketType;
using sotc::pm4::Status;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

// Type-3 header as the writers build it: 0xC0000000 | opcode << 8 | (payload - 1) << 16.
constexpr std::uint32_t command(std::uint16_t opcode, std::uint32_t payload_dwords) {
    return (3u << 30) | (((payload_dwords - 1u) & 0x3FFFu) << 16) |
           ((static_cast<std::uint32_t>(opcode) & 0xFFu) << 8);
}

constexpr std::uint32_t register_write(std::uint16_t index, std::uint32_t payload_dwords) {
    return (1u << 30) | (((payload_dwords - 1u) & 0x3FFFu) << 16) | index;
}

// The pinned image's executable segment ends here (PT_LOAD 0 filesz); writer RVAs must land inside.
constexpr std::uint32_t kImageTextEnd = 0x1593978u;

std::vector<std::uint32_t> stream_with(std::uint32_t header, std::uint32_t payload_dwords,
                                      std::uint32_t fill = 0) {
    std::vector<std::uint32_t> words;
    words.push_back(header);
    for (std::uint32_t i = 0; i < payload_dwords; ++i) {
        words.push_back(fill + i);
    }
    return words;
}

void test_table_shape() {
    check("row count", sotc::gnm::row_count() == 27);
    const OpcodeRow* rows = sotc::gnm::rows();
    check("rows pointer", rows != nullptr);
    if (rows == nullptr) {
        return;
    }
    std::uint32_t previous = 0;
    bool ascending = true;
    bool unique = true;
    for (std::size_t i = 0; i < sotc::gnm::row_count(); ++i) {
        const OpcodeRow& row = rows[i];
        if (i > 0 && row.opcode <= previous) {
            ascending = false;
            unique = false;
        }
        previous = row.opcode;
        check("row name", row.name != nullptr && row.name[0] != '\0');
        check("row evidence", row.evidence != nullptr && row.evidence[0] != '\0');
        check("row has sites", row.writer_sites > 0);
        check("row rva order", row.first_site_rva <= row.last_site_rva);
        check("row rva in text", row.last_site_rva < kImageTextEnd);
        check("row payload bounds", row.min_payload >= 1 && row.min_payload <= row.max_payload);
        check("row payload cap", row.max_payload <= 0x40);
        if (row.size_kind == SizeKind::fixed) {
            check("fixed rows are exact", row.min_payload == row.max_payload);
            check("fixed row mask is one bit",
                  row.payload_mask == (std::uint64_t{1} << (row.min_payload - 1)));
        }
        if (row.size_kind != SizeKind::unbounded) {
            check("stored-size mask covers the bounds",
                  (row.payload_mask & (std::uint64_t{1} << (row.min_payload - 1))) != 0 &&
                      (row.payload_mask & (std::uint64_t{1} << (row.max_payload - 1))) != 0);
        }
        if (row.size_kind == SizeKind::unbounded) {
            check("unbounded rows are open", row.min_payload <= row.max_payload);
        }
        if (row.family == Family::register_write) {
            check("register rows carry at least an index", row.min_payload >= 1);
        }
    }
    check("opcodes ascending", ascending);
    check("opcodes unique", unique);
}

void test_lookup() {
    const OpcodeRow* rows = sotc::gnm::rows();
    bool all_found = true;
    for (std::size_t i = 0; i < sotc::gnm::row_count(); ++i) {
        const OpcodeRow* found = sotc::gnm::find(rows[i].opcode);
        if (found != &rows[i]) {
            all_found = false;
        }
    }
    check("every row is findable", all_found);
    check("missing opcode", sotc::gnm::find(0x11) == nullptr);
    check("opcode above 8 bits", sotc::gnm::find(0x1FF) == nullptr);
    check("opcode 0 stays unknown", sotc::gnm::find(0x00) == nullptr);
}

void test_known_headers_accepted() {
    const OpcodeRow* rows = sotc::gnm::rows();
    bool all_ok = true;
    for (std::size_t i = 0; i < sotc::gnm::row_count(); ++i) {
        const OpcodeRow& row = rows[i];
        const std::int32_t sizes[2] = {row.min_payload, row.max_payload};
        for (const std::int32_t payload : sizes) {
            HeaderInfo info{};
            const Verdict verdict = sotc::gnm::classify(command(row.opcode, payload), &info);
            if (verdict != Verdict::ok) {
                all_ok = false;
            }
            if (info.opcode != row.opcode || info.payload_dwords != payload) {
                all_ok = false;
            }
        }
    }
    check("every row accepts its own payload bounds", all_ok);
}

void test_unknown_opcode_rejected() {
    // 0x11 has no writer site anywhere in the pinned image.
    HeaderInfo info{};
    check("unknown opcode", sotc::gnm::classify(command(0x11, 1), &info) == Verdict::unknown_opcode);
    check("unknown opcode info", info.opcode == 0x11 && info.payload_dwords == 1);
    // Opcode 0x00 is deliberately absent: the constant is ambiguous in this image.
    check("opcode 0 rejected", sotc::gnm::classify(command(0x00, 1)) == Verdict::unknown_opcode);
}

void test_malformed_header_rejected() {
    const std::uint32_t opcode_88 = command(0x88, 1);
    check("type 1 header", sotc::gnm::classify(register_write(0x2000, 1)) ==
                               Verdict::not_command_packet);
    check("type 0 header", sotc::gnm::classify(0u) == Verdict::not_command_packet);
    check("type 2 header", sotc::gnm::classify(2u << 30) == Verdict::not_command_packet);
    // Bits[7:1] of a command header are reserved and must be zero.
    check("reserved bit 1", sotc::gnm::classify(opcode_88 | 0x2u) == Verdict::reserved_bits_set);
    check("reserved byte", sotc::gnm::classify(opcode_88 | 0x80u) == Verdict::reserved_bits_set);
    // Bit 0 is NOT reserved: writer 0x86b7dd ORs a runtime flag into it on opcode 0x8c.
    check("low flag bit accepted",
          sotc::gnm::classify(command(0x8c, 5) | 0x1u) == Verdict::ok);
    // A header that claims more payload than any writer produces.
    check("oversized payload", sotc::gnm::classify(command(0x88, 2)) ==
                                   Verdict::payload_size_mismatch);
    check("count at field maximum", sotc::gnm::classify(
              (3u << 30) | (0x3FFFu << 16) | (0x88u << 8)) == Verdict::payload_size_mismatch);
}

void test_payload_ranges() {
    // Fixed: opcode 0x13 is always one payload dword, 0x3c always six.
    check("0x13 accepts 1", sotc::gnm::classify(command(0x13, 1)) == Verdict::ok);
    check("0x13 rejects 2", sotc::gnm::classify(command(0x13, 2)) ==
                                 Verdict::payload_size_mismatch);
    check("0x3c accepts 6", sotc::gnm::classify(command(0x3c, 6)) == Verdict::ok);
    check("0x3c rejects 5", sotc::gnm::classify(command(0x3c, 5)) ==
                                 Verdict::payload_size_mismatch);
    // Closed: opcode 0x46 stores one or three payload dwords, never two or four.
    check("0x46 accepts 1", sotc::gnm::classify(command(0x46, 1)) == Verdict::ok);
    check("0x46 accepts 3", sotc::gnm::classify(command(0x46, 3)) == Verdict::ok);
    check("0x46 rejects 2", sotc::gnm::classify(command(0x46, 2)) ==
                                 Verdict::payload_size_mismatch);
    check("0x46 rejects 4", sotc::gnm::classify(command(0x46, 4)) ==
                                 Verdict::payload_size_mismatch);
    // Unbounded: 0x69 patches its count at runtime, so only the lower bound is pinned.
    check("0x69 accepts 1", sotc::gnm::classify(command(0x69, 1)) == Verdict::ok);
    check("0x69 accepts 30", sotc::gnm::classify(command(0x69, 30)) == Verdict::ok);
    // ...while 0x8d's only writers patch a count that is already 3.
    check("0x8d rejects 2", sotc::gnm::classify(command(0x8d, 2)) ==
                                 Verdict::payload_size_mismatch);
    check("0x8d accepts 3", sotc::gnm::classify(command(0x8d, 3)) == Verdict::ok);
}

void test_names() {
    check("family name", std::string_view(sotc::gnm::family_name(Family::register_write)) ==
                             std::string_view("register_write"));
    check("size kind name", std::string_view(sotc::gnm::size_kind_name(SizeKind::unbounded)) ==
                                std::string_view("unbounded"));
    check("verdict name", std::string_view(sotc::gnm::verdict_name(Verdict::unknown_opcode)) ==
                              std::string_view("unknown_opcode"));
}

void test_validate_accepts_grounded_stream() {
    // A register write (0x69: index 0x105 + one value) between two framed non-command packets.
    std::vector<std::uint32_t> words;
    words.push_back(0u);                             // nop
    words.push_back(register_write(0x2000, 2));      // type 1, two dwords
    words.push_back(0x11111111u);
    words.push_back(0x22222222u);
    words.push_back(command(0x69, 2));               // register write
    words.push_back(0x105u);
    words.push_back(0xdeadbeefu);
    words.push_back(command(0x2a, 1));
    words.push_back(0x0u);
    words.push_back(command(0x50, 6));
    for (std::uint32_t i = 0; i < 6; ++i) {
        words.push_back(0x5000u + i);
    }

    const sotc::gnm::ValidationSummary summary =
        sotc::gnm::validate(words.data(), words.size());
    check("stream decodes", summary.decode == Status::ok);
    check("stream verdict", summary.verdict == Verdict::ok);
    check("stream command packets", summary.command_packets == 3);
    check("stream known opcodes", summary.known_opcodes == 3);
    check("stream rejected", summary.rejected == 0);
}

void test_validate_rejects_unknown_opcode() {
    std::vector<std::uint32_t> words;
    words.push_back(command(0x88, 1));
    words.push_back(0x1234u);
    words.push_back(command(0x11, 1));  // no writer site
    words.push_back(0x0u);
    const sotc::gnm::ValidationSummary summary =
        sotc::gnm::validate(words.data(), words.size());
    check("unknown opcode verdict", summary.verdict == Verdict::unknown_opcode);
    check("unknown opcode index", summary.bad_header_index == 2);
    check("unknown opcode counted", summary.unknown_opcodes_seen == 1);
    check("unknown opcode stops", summary.known_opcodes == 1);
}

void test_validate_rejects_bad_size() {
    std::vector<std::uint32_t> words = stream_with(command(0x13, 2), 2);
    const sotc::gnm::ValidationSummary summary =
        sotc::gnm::validate(words.data(), words.size());
    check("bad size verdict", summary.verdict == Verdict::payload_size_mismatch);
    check("bad size index", summary.bad_header_index == 0);
    check("bad size is a sink rejection", summary.decode == Status::sink_rejected);
}

void test_validate_reports_framing_errors() {
    // Header claims six payload dwords, only two are present.
    std::vector<std::uint32_t> words = stream_with(command(0x3c, 6), 2);
    const sotc::gnm::ValidationSummary summary =
        sotc::gnm::validate(words.data(), words.size());
    check("framing status", summary.decode == Status::truncated_payload);
    check("framing index", summary.bad_header_index == 0);
    check("framing keeps verdict ok", summary.verdict == Verdict::ok);

    const sotc::gnm::ValidationSummary empty = sotc::gnm::validate(nullptr, 0);
    check("empty buffer", empty.decode == Status::empty_buffer);
}

void test_validate_packet_cap() {
    std::vector<std::uint32_t> words;
    for (int i = 0; i < 4; ++i) {
        words.push_back(command(0x88, 1));
        words.push_back(static_cast<std::uint32_t>(i));
    }
    Limits limits{};
    limits.max_packets = 2;
    const sotc::gnm::ValidationSummary summary =
        sotc::gnm::validate(words.data(), words.size(), limits);
    check("packet cap status", summary.decode == Status::too_many_packets);
}

}  // namespace

int main() {
    test_table_shape();
    test_lookup();
    test_known_headers_accepted();
    test_unknown_opcode_rejected();
    test_malformed_header_rejected();
    test_payload_ranges();
    test_names();
    test_validate_accepts_grounded_stream();
    test_validate_rejects_unknown_opcode();
    test_validate_rejects_bad_size();
    test_validate_reports_framing_errors();
    test_validate_packet_cap();

    if (failures == 0) {
        std::printf("OK test_gnm_opcode_table\n");
        return 0;
    }
    std::fprintf(stderr, "test_gnm_opcode_table: %d failure(s)\n", failures);
    return 1;
}
