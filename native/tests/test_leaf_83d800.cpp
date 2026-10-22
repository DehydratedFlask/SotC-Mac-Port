// The recovered cells are compared with an independent oracle (explicit dword table) and with a
// poisoned buffer to prove that bytes no store covers are never written. --dump feeds
// tools/lift/check_f83d800.py (binary-vs-native tie).
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/leaf_83d800.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
constexpr std::size_t kSize = 284;
constexpr std::uint8_t kPoison = 0xAA;
struct Dword { std::size_t offset; std::uint32_t value; };
// The six stores of the original (cell offset from 0x3293400, value).
constexpr std::array<Dword, 6> kDwords = {{
    {0x000, 0x00000000u}, {0x058, 0xffffffffu}, {0x060, 0x00000000u},
    {0x0b8, 0xffffffffu}, {0x0c0, 0x00000000u}, {0x118, 0xffffffffu},
}};

std::uint32_t read_u32(const std::array<std::uint8_t, kSize>& cells, std::size_t offset) {
    std::uint32_t v = 0;
    std::memcpy(&v, cells.data() + offset, 4);
    return v;
}
bool covered(std::size_t i) {
    for (const auto& d : kDwords) if (i >= d.offset && i < d.offset + 4) return true;
    return false;
}
}  // namespace

int main(int argc, char**) {
    using namespace sotc;
    Leaf83d800 g{};
    init_leaf_83d800(g);
    for (const auto& d : kDwords) CHECK(read_u32(g.cells, d.offset) == d.value);
    for (std::size_t i = 0; i != kSize; ++i) if (!covered(i)) CHECK(g.cells[i] == 0);

    // Poisoned buffer: covered dwords are rewritten, every other byte stays untouched.
    Leaf83d800 p{};
    p.cells.fill(kPoison);
    init_leaf_83d800(p);
    std::size_t covered_count = 0;
    for (std::size_t i = 0; i != kSize; ++i) {
        if (covered(i)) {
            ++covered_count;
            std::uint32_t expect = 0;
            for (const auto& d : kDwords) if (i >= d.offset && i < d.offset + 4) expect = d.value;
            CHECK(p.cells[i] == static_cast<std::uint8_t>(expect >> (8 * (i & 3)) ));
        } else {
            CHECK(p.cells[i] == kPoison);
        }
    }
    CHECK(covered_count == 24);
    // The old contents are overwritten, not merged: a buffer of the opposite polarity ends identical.
    Leaf83d800 q{};
    q.cells.fill(0xFF);
    init_leaf_83d800(q);
    for (const auto& d : kDwords) CHECK(read_u32(q.cells, d.offset) == d.value);
    init_leaf_83d800(p);   // idempotent
    for (std::size_t i = 0; i != kSize; ++i) if (!covered(i)) CHECK(p.cells[i] == kPoison);

    if (argc > 1) {
        std::printf("{\"rva\":%u,\"body_length\":%u,\"base\":%llu,\"bytes\":\"",
                    0x83d800u, 61u, static_cast<unsigned long long>(0x3293400ull));
        for (auto byte : g.cells) std::printf("%02x", byte);
        std::printf("\",\"bytes_poisoned\":\"");
        for (auto byte : p.cells) std::printf("%02x", byte);
        std::printf("\"}\n");
    } else {
        std::puts("leaf_83d800: all checks passed");
    }
    return 0;
}
