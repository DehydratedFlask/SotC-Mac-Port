// The recovered cells are compared with the exact bytes the original stores, lane by lane against
// an independent oracle, and with a poisoned buffer to prove that bytes no store covers are never
// written. --dump feeds tools/lift/check_f8a0440.py (binary-vs-native tie).
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/leaf_8a0440.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
constexpr std::size_t kSize = 48;
constexpr std::array<std::uint8_t, kSize> kCells = {
    0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde,
    0xb3, 0x85, 0x6f, 0x20, 0xb3, 0x85, 0x6f, 0x20, 0xb3, 0x85, 0x6f, 0x20, 0xb3, 0x85, 0x6f, 0x20,
    0x25, 0xb5, 0x68, 0x57, 0x25, 0xb5, 0x68, 0x57, 0x25, 0xb5, 0x68, 0x57, 0x25, 0xb5, 0x68, 0x57,
};
// 1 where some store of the original covers the byte, 0 where the cell is never written.
constexpr std::array<std::uint8_t, kSize> kCovered = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
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
    Leaf8a0440 g{};
    init_leaf_8a0440(g);
    CHECK(g.cells == kCells);
    // Per-store lane oracle (independent of the 64-bit chunks used by the implementation).
    CHECK(lanes<4>(g.cells, 0x000) == (std::array<std::uint32_t, 4>{0xdeadbeefu, 0xdeadbeefu, 0xdeadbeefu, 0xdeadbeefu}));
    CHECK(lanes<4>(g.cells, 0x010) == (std::array<std::uint32_t, 4>{0x206f85b3u, 0x206f85b3u, 0x206f85b3u, 0x206f85b3u}));
    CHECK(lanes<4>(g.cells, 0x020) == (std::array<std::uint32_t, 4>{0x5768b525u, 0x5768b525u, 0x5768b525u, 0x5768b525u}));

    // Unguarded: a second call over a poisoned buffer rewrites every covered byte to the same
    // value and leaves every uncovered byte untouched.
    Leaf8a0440 p{};
    p.cells.fill(kPoison);
    init_leaf_8a0440(p);
    std::size_t covered = 0;
    for (std::size_t i = 0; i != kSize; ++i) {
        if (kCovered[i]) {
            CHECK(p.cells[i] == kCells[i]);
            ++covered;
        } else {
            CHECK(p.cells[i] == kPoison);
        }
    }
    CHECK(covered == 48);
    init_leaf_8a0440(p);   // idempotent
    for (std::size_t i = 0; i != kSize; ++i) CHECK(p.cells[i] == (kCovered[i] ? kCells[i] : kPoison));

    if (argc > 1) {
        std::printf("{\"rva\":%u,\"body_length\":%u,\"base\":%llu,\"bytes\":\"",
                    0x8a0440u, 49u, static_cast<unsigned long long>(0x3294f40ull));
        for (auto byte : g.cells) std::printf("%02x", byte);
        std::printf("\",\"bytes_poisoned\":\"");
        for (auto byte : p.cells) std::printf("%02x", byte);
        std::printf("\"}\n");
    } else {
        std::puts("leaf_8a0440: all checks passed");
    }
    return 0;
}
