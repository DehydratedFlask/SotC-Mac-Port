#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/arena_accounting.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

constexpr std::size_t kFixtureBytes = sotc::kF19E00RawBytes + 16;

struct FixtureCase {
    const char* name;
    std::uint64_t requested;
    std::uint32_t mode;
    std::array<std::byte, kFixtureBytes> before;
    std::array<std::byte, kFixtureBytes> after;
};

template <std::size_t N>
std::uint64_t read_u64_le(const std::array<std::byte, N>& raw,
                          std::size_t offset) {
    std::uint64_t value = 0;
    for (std::size_t i = 0; i < sizeof(value); ++i) {
        value |= static_cast<std::uint64_t>(std::to_integer<unsigned char>(raw[offset + i]))
                 << (i * 8);
    }
    return value;
}

template <std::size_t N>
void write_u64_le(std::array<std::byte, N>& raw,
                  std::size_t offset, std::uint64_t value) {
    for (std::size_t i = 0; i < sizeof(value); ++i) {
        raw[offset + i] = std::byte{static_cast<unsigned char>(value >> (i * 8))};
    }
}

FixtureCase make_case(const char* name, std::uint64_t requested, std::uint32_t mode,
                      std::uint64_t at80, std::uint64_t at88, std::uint64_t at90) {
    FixtureCase fixture{name, requested, mode, {}, {}};
    for (std::size_t i = 0; i < fixture.before.size(); ++i) {
        fixture.before[i] = std::byte{static_cast<unsigned char>((i * 37 + 0x5b) & 0xff)};
    }
    write_u64_le(fixture.before, sotc::kF19E00Offset80, at80);
    write_u64_le(fixture.before, sotc::kF19E00Offset88, at88);
    write_u64_le(fixture.before, sotc::kF19E00Offset90, at90);
    fixture.after = fixture.before;
    return fixture;
}

void encode_hex(const std::array<std::byte, kFixtureBytes>& bytes,
                char (&output)[kFixtureBytes * 2 + 1]) {
    constexpr char kHex[] = "0123456789abcdef";
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        const auto value = std::to_integer<unsigned char>(bytes[i]);
        output[i * 2] = kHex[value >> 4];
        output[i * 2 + 1] = kHex[value & 0x0f];
    }
    output[kFixtureBytes * 2] = '\0';
}

void verify_case(FixtureCase& fixture) {
    CHECK(sotc::apply_f19e00_accounting(fixture.after, fixture.requested, fixture.mode));

    const std::uint64_t next = read_u64_le(fixture.before, sotc::kF19E00Offset80) + fixture.requested;
    CHECK(read_u64_le(fixture.after, sotc::kF19E00Offset80) == next);
    const std::uint64_t before88 = read_u64_le(fixture.before, sotc::kF19E00Offset88);
    CHECK(read_u64_le(fixture.after, sotc::kF19E00Offset88) == (next > before88 ? next : before88));
    const std::uint64_t before90 = read_u64_le(fixture.before, sotc::kF19E00Offset90);
    const std::uint64_t expected90 = fixture.mode == 2 ? before90 - fixture.requested : before90;
    CHECK(read_u64_le(fixture.after, sotc::kF19E00Offset90) == expected90);

    for (std::size_t i = 0; i < fixture.before.size(); ++i) {
        if (i < sotc::kF19E00Offset80 || i >= sotc::kF19E00RawBytes) {
            CHECK(fixture.after[i] == fixture.before[i]);
        }
    }
}

void emit_case(const FixtureCase& fixture, bool last) {
    char before[kFixtureBytes * 2 + 1];
    char after[kFixtureBytes * 2 + 1];
    encode_hex(fixture.before, before);
    encode_hex(fixture.after, after);
    std::printf("    {\"name\":\"%s\",\"requested\":%llu,\"mode\":%u,"
                "\"before\":\"%s\",\"after\":\"%s\"}%s\n",
                fixture.name,
                static_cast<unsigned long long>(fixture.requested),
                fixture.mode, before, after, last ? "" : ",");
}

}  // namespace

int main(int argc, char** argv) {
    std::array<FixtureCase, 6> cases{
        make_case("mode2_updates", 0x10, 2, 0x20, 0x1f, 0x80),
        make_case("mode0_preserves_90", 3, 0, 100, 128, 0x1122334455667788ull),
        make_case("mode3_preserves_90", 7, 3, 0x100, 0x100, 0x8877665544332211ull),
        make_case("unsigned_high_water", 1, 0, 0x7fffffffffffffffull,
                  0x7ffffffffffffffeull, 0x123456789abcdef0ull),
        make_case("wrap_add_and_subtract", 8, 2, 0xfffffffffffffffcull, 5, 4),
        make_case("zero_request_repairs_peak", 0, 0, 0x100, 0xff, 0x123),
    };

    for (auto& fixture : cases) verify_case(fixture);

    std::array<std::byte, sotc::kF19E00RawBytes - 1> too_short;
    too_short.fill(std::byte{0xa5});
    const auto short_before = too_short;
    CHECK(!sotc::apply_f19e00_accounting(too_short, 1, 2));
    CHECK(too_short == short_before);

    std::array<std::byte, sotc::kF19E00RawBytes> exact_size{};
    write_u64_le(exact_size, sotc::kF19E00Offset80, 1);
    write_u64_le(exact_size, sotc::kF19E00Offset88, 1);
    write_u64_le(exact_size, sotc::kF19E00Offset90, 5);
    CHECK(sotc::apply_f19e00_accounting(exact_size, 2, 2));
    CHECK(read_u64_le(exact_size, sotc::kF19E00Offset80) == 3);
    CHECK(read_u64_le(exact_size, sotc::kF19E00Offset88) == 3);
    CHECK(read_u64_le(exact_size, sotc::kF19E00Offset90) == 3);

    if (argc == 2 && std::strcmp(argv[1], "--json") == 0) {
        std::puts("{\"cases\":[");
        for (std::size_t i = 0; i < cases.size(); ++i) {
            emit_case(cases[i], i + 1 == cases.size());
        }
        std::puts("]}");
    } else {
        CHECK(argc == 1);
        std::puts("arena_accounting: all checks passed");
    }
    return 0;
}
