// The recovered cells are compared with the exact bytes the original stores, lane by lane against
// an independent oracle, and with a poisoned buffer to prove that bytes no store covers are never
// written. --dump feeds tools/lift/check_fc05980.py (binary-vs-native tie).
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/leaf_c05980.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
constexpr std::size_t kSize = 18;
constexpr std::array<std::uint8_t, kSize> kCells = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x04, 0x04,
};
// 1 where some store of the original covers the byte, 0 where the cell is never written.
constexpr std::array<std::uint8_t, kSize> kCovered = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01,
};
constexpr std::uint8_t kPoison = 0xAA;

template <std::size_t N>
std::array<std::uint32_t, N> lanes(const std::array<std::uint8_t, kSize>& cells, std::size_t offset) {
    std::array<std::uint32_t, N> out{};
    std::memcpy(out.data(), cells.data() + offset, N * 4);
    return out;
}
}  // namespace

int main(int argc, char**) {
    using namespace sotc;
    LeafC05980 g{};
    init_leaf_c05980(g);
    CHECK(g.cells == kCells);
    // Per-store lane oracle (independent of the 64-bit chunks used by the implementation).
    CHECK(g.cells[0x010] == 0x04);
    CHECK(g.cells[0x011] == 0x04);
    CHECK(lanes<4>(g.cells, 0x000) == (std::array<std::uint32_t, 4>{0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u}));

    // Unguarded: a second call over a poisoned buffer rewrites every covered byte to the same
    // value and leaves every uncovered byte untouched.
    LeafC05980 p{};
    p.cells.fill(kPoison);
    init_leaf_c05980(p);
    std::size_t covered = 0;
    for (std::size_t i = 0; i != kSize; ++i) {
        if (kCovered[i]) {
            CHECK(p.cells[i] == kCells[i]);
            ++covered;
        } else {
            CHECK(p.cells[i] == kPoison);
        }
    }
    CHECK(covered == 18);
    init_leaf_c05980(p);   // idempotent
    for (std::size_t i = 0; i != kSize; ++i) CHECK(p.cells[i] == (kCovered[i] ? kCells[i] : kPoison));

    if (argc > 1) {
        std::printf("{\"rva\":%u,\"body_length\":%u,\"base\":%llu,\"bytes\":\"",
                    0xc05980u, 31u, static_cast<unsigned long long>(0x3351290ull));
        for (auto byte : g.cells) std::printf("%02x", byte);
        std::printf("\",\"bytes_poisoned\":\"");
        for (auto byte : p.cells) std::printf("%02x", byte);
        std::printf("\"}\n");
    } else {
        std::puts("leaf_c05980: all checks passed");
    }
    return 0;
}
