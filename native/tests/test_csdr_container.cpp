#include "core/csdr_container.hpp"

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <utility>
#include <vector>

using namespace sotc::csdr;
namespace {
unsigned checks = 0;
void check(bool condition, const char* message) {
    ++checks;
    if (!condition) {
        std::fprintf(stderr, "FAIL %s (check %u)\n", message, checks);
        std::exit(1);
    }
}
void put32(std::vector<std::uint8_t>& bytes, std::size_t offset, std::uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) bytes.at(offset + i) = (value >> (i * 8)) & 255;
}
void append32(std::vector<std::uint8_t>& bytes, std::uint32_t value) {
    const auto offset = bytes.size();
    bytes.resize(offset + 4);
    put32(bytes, offset, value);
}
// Entirely invented bytes. No game-derived fixture or instruction implementation is included.
std::vector<std::uint8_t> fixture(const std::vector<std::pair<unsigned, unsigned>>& stages) {
    std::vector<std::uint8_t> bytes(64);
    put32(bytes, 0, stages.size());
    for (auto [type, dwords] : stages) {
        const auto length = dwords * 4;
        append32(bytes, 0xbeeb03ff);
        append32(bytes, (length + 8) / 8 - 1);
        for (unsigned i = 2; i + 1 < dwords; ++i) append32(bytes, 0xbf800000);
        append32(bytes, 0xbf810000);
        bytes.resize(bytes.size() + 8);
        const std::array<std::uint8_t, 8> magic{'O', 'r', 'b', 'S', 'h', 'd', 'r', 7};
        bytes.insert(bytes.end(), magic.begin(), magic.end());
        append32(bytes, 1 | (type << 2) | (1 << 6) | (length << 8));
        append32(bytes, 0x050f0102);
        append32(bytes, 0x55667788);
        append32(bytes, 0x11223344);
        append32(bytes, 0xdeadbeef);
    }
    return bytes;
}
void expect(const std::vector<std::uint8_t>& bytes, Status status, const char* message) {
    const auto result = parse(bytes);
    if (result.status != status)
        std::fprintf(stderr, "expected %s; got %s at %zu\n", status_name(status),
                     status_name(result.status), result.error_offset);
    check(result.status == status, message);
    check(result.programs.empty(), "failed imports expose no partial program list");
}
} // namespace

int main() {
    const auto good = fixture({{1, 20}, {0, 12}});
    const auto result = parse(good);
    check(bool(result), "two-stage fixture accepted");
    check(result.declared_count == 2 && result.observed_count == 2 && result.programs.size() == 2,
          "container count");
    const auto& first = result.programs[0];
    check(first.code_offset == 64 && first.info_offset == 152 && first.token_immediate == 10,
          "first offsets and token immediate");
    check(first.info.type == 1 && first.info.version == 7 && first.info.source_type == 1 &&
          first.info.length == 80 && first.info.pssl_or_cg && !first.info.cached,
          "first BinaryInfo fields");
    check(first.info.chunk_usage_base_offset_in_dw == 2 && first.info.num_input_usage_slots == 1 &&
          first.info.usage_flags == 15 && first.info.reserved == 5,
          "usage metadata retained without interpretation");
    check(first.info.shader_hash == 0x1122334455667788 && first.info.crc32 == 0xdeadbeef,
          "hash and CRC metadata");
    check(result.programs[1].code_offset == 180 && result.programs[1].info_offset == 236 &&
          result.programs[1].info.type == 0 && result.programs[1].info.length == 48,
          "second offsets and BinaryInfo fields");
    const auto code = code_bytes(good, first);
    check(code.size() == 80 && code.data() == good.data() + 64, "bounded borrowed code view");

    // Exercise every prefix boundary, including the partial second program and BinaryInfo fields.
    for (std::size_t length = 0; length < good.size(); ++length) {
        const auto truncated = parse(good.data(), length);
        check(!truncated && truncated.programs.empty(), "all truncated prefixes fail");
    }
    check(parse(nullptr, 1).status == Status::invalid_input, "null/nonempty input rejected");
    check(parse(nullptr, 0).status == Status::too_short, "empty input rejected");
    auto unaligned = good;
    unaligned.insert(unaligned.begin(), 0xff);
    check(bool(parse(std::span<const std::uint8_t>(unaligned).subspan(1))),
          "host buffer alignment is irrelevant");

    expect(std::vector<std::uint8_t>(32), Status::no_programs, "missing programs");
    for (auto head : {std::vector<std::uint8_t>{0x8c, 0x06}, {0x8c, 0x0a},
                      {0xcc, 0x06}, {0xcc, 0x0a}, {0xcc, 0x7f}})
        expect(head, Status::unsupported_compression, "block stream fails visibly");
    for (unsigned count : {140u, 204u, 1676u, 2700u, 1740u, 2764u}) {
        const auto many = fixture(std::vector<std::pair<unsigned, unsigned>>(count, {1, 20}));
        const auto parsed = parse(many);
        check(bool(parsed) && parsed.declared_count == count && parsed.programs.size() == count,
              "complete valid raw structure takes precedence over a compressed prefix");
    }

    auto bad = good;
    put32(bad, 0, 1);
    expect(bad, Status::count_mismatch, "count mismatch");
    put32(bad, 0, 0xffffffff);
    expect(bad, Status::count_mismatch, "huge count cannot drive allocation");
    bad = good;
    bad.resize(good.size() - 1);
    expect(bad, Status::truncated_binary_info, "truncated BinaryInfo");
    bad = good;
    bad[first.info_offset + 7] = 8;
    expect(bad, Status::unsupported_version, "unknown version");
    bad = good;
    bad[first.info_offset + 8] ^= 0x40;
    expect(bad, Status::unsupported_source_type, "unknown source type");
    bad = good;
    bad[first.info_offset + 8] = (bad[first.info_offset + 8] & 0xc3) | (3 << 2);
    expect(bad, Status::unsupported_shader_type, "unknown stage");
    bad = good;
    put32(bad, first.code_offset + 4, 0xffffffff);
    expect(bad, Status::missing_token, "overflowing token target rejected");
    bad = good;
    put32(bad, first.code_offset + 8, 0xbeeb03ff);
    put32(bad, first.code_offset + 12, first.token_immediate - 1);
    expect(bad, Status::ambiguous_token, "ambiguous token mapping");
    for (std::uint32_t length : {0u, 4u, 8u, 79u, 92u, 0xffffffu}) {
        bad = good;
        put32(bad, first.info_offset + 8, 0x45 | (length << 8));
        expect(bad, Status::invalid_length, "invalid declared length");
    }
    bad = good;
    put32(bad, first.code_offset + first.info.length - 4, 0);
    expect(bad, Status::missing_endpgm, "bad terminal word");

    // A second token mapping from inside the previous program is not a second importable span.
    bad = good;
    put32(bad, result.programs[1].code_offset, 0);
    put32(bad, first.code_offset + 8, 0xbeeb03ff);
    put32(bad, first.code_offset + 12,
          (result.programs[1].info_offset - first.code_offset - 8) / 8 - 1);
    // The fixture offsets have different residues modulo 8; align both second spans together.
    bad.insert(bad.begin() + 180, 4, 0);
    put32(bad, first.code_offset + 12, (240 - first.code_offset - 8) / 8 - 1);
    expect(bad, Status::invalid_span, "overlapping program spans rejected");

    auto gs = fixture({{5, 20}, {6, 12}});
    put32(gs, 0, 1);
    expect(gs, Status::unsupported_gs_double_program, "GS double-program exception named");
    gs[152 + 7] = 8;
    expect(gs, Status::unsupported_version, "GS exception does not hide malformed programs");
    gs = fixture({{5, 20}, {6, 12}});
    put32(gs, 0, 1);
    put32(gs, 180 + 48 - 4, 0);
    expect(gs, Status::missing_endpgm, "both exception programs must validate");
    expect(fixture({{5, 20}}), Status::unsupported_shader_type, "type 5 is not accepted alone");
    expect(fixture({{6, 20}}), Status::unsupported_shader_type, "type 6 is not accepted alone");
    gs = fixture({{2, 20}, {1, 12}});
    put32(gs, 0, 1);
    expect(gs, Status::count_mismatch, "other type pairs cannot use the exception diagnostic");
    for (unsigned type : {0u, 1u, 2u, 4u, 7u, 8u})
        check(bool(parse(fixture({{type, 20}}))), "observed stage codes retained");

    // Corrupt every bit of structural magic, target, version, length and terminal-word fields.
    unsigned rejected = 0;
    const std::array<std::pair<std::size_t, std::size_t>, 5> ranges{{
        {64, 8}, {152, 8}, {161, 3}, {140, 4}, {0, 4}}};
    for (auto [offset, length] : ranges) {
        for (std::size_t i = 0; i < length; ++i) {
            for (unsigned bit = 0; bit < 8; ++bit) {
                bad = good;
                bad[offset + i] ^= 1u << bit;
                check(!parse(bad), "structural bit flip rejected");
                ++rejected;
            }
        }
    }
    // Hash/CRC mutation is intentionally metadata-only: no algorithm/coverage claim is made.
    bad = good;
    bad[152 + 16] ^= 1;
    bad[152 + 24] ^= 1;
    const auto metadata_changed = parse(bad);
    check(bool(metadata_changed) && metadata_changed.programs[0].info.shader_hash ==
                                       (first.info.shader_hash ^ 1) &&
          metadata_changed.programs[0].info.crc32 == (first.info.crc32 ^ 1),
          "checksum metadata is not falsely authenticated");
    Program forged = first;
    forged.code_offset = std::numeric_limits<std::size_t>::max() - 1;
    check(code_bytes(good, forged).empty(), "forged offset cannot wrap");
    forged = first;
    forged.info.length = 0xffffffff;
    check(code_bytes(good, forged).empty(), "forged length cannot escape input");
    forged = first;
    forged.info_offset = 2;
    check(code_bytes(good, forged).empty(), "forged backward info rejected");
    std::printf("csdr_container: %u checks, %u structural bit flips and %zu truncated prefixes rejected\n",
                checks, rejected, good.size());
    return 0;
}
