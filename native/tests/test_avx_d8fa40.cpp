// The recovered cells are compared with the exact bytes the original stores; a few cells also
// have independent IEEE/mask oracles. --dump feeds tools/lift/check_fd8fa40.py (binary-vs-native tie).
#include <array>
#include <bit>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/avx_d8fa40.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
constexpr std::array<std::uint8_t, 96> kCells = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80,
    0x5f, 0x70, 0x89, 0x30, 0x5f, 0x70, 0x89, 0x30, 0x5f, 0x70, 0x89, 0x30, 0x5f, 0x70, 0x89, 0x30,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40,
    0x95, 0xbf, 0xd6, 0x33, 0x95, 0xbf, 0xd6, 0x33, 0x95, 0xbf, 0xd6, 0x33, 0x95, 0xbf, 0xd6, 0x33,
};
// Byte ranges the original writes; bytes outside them are never touched (BSS zero in the original).
constexpr std::size_t kStoreOffsets[] = {0, 64, 80, 16, 32, 48};
constexpr std::size_t kStoreWidths[] = {16, 16, 16, 16, 16, 16};
constexpr std::size_t kGapOffset = 96;   // first byte no store covers (== size when none)

std::array<std::uint32_t, 4> lanes_at(const std::array<std::uint8_t, 96>& cells, std::size_t offset) {
    std::array<std::uint32_t, 4> lanes{};
    std::memcpy(lanes.data(), cells.data() + offset, 16);
    return lanes;
}
}  // namespace

int main(int argc, char**) {
    using namespace sotc;
    AvxD8fa40 g{};
    init_avx_d8fa40(g);
    CHECK(g.cells == kCells);
    // Independent oracles for two decoded cells.
    CHECK((lanes_at(g.cells, 0x00) == std::array<std::uint32_t, 4>{~0u, ~0u, ~0u, 0}));
    CHECK((lanes_at(g.cells, 0x10) == std::array<std::uint32_t, 4>{0x80000000u, 0x80000000u, 0x80000000u, 0x80000000u}));
    CHECK((lanes_at(g.cells, 0x40) == std::array<std::uint32_t, 4>{0x40000000u, 0x40000000u, 0x40000000u, 0x40000000u}));

    // Unguarded: a second call rewrites every covered byte; gap bytes are not written at all.
    AvxD8fa40 h = g;
    h.cells.fill(0xAA);
    init_avx_d8fa40(h);
    std::size_t covered = 0;
    for (std::size_t store = 0; store != std::size(kStoreOffsets); ++store) {
        for (std::size_t i = 0; i != kStoreWidths[store]; ++i) {
            CHECK(h.cells[kStoreOffsets[store] + i] == kCells[kStoreOffsets[store] + i]);
        }
        covered += kStoreWidths[store];
    }
    CHECK(covered <= kCells.size());
    if (kGapOffset < kCells.size()) CHECK(h.cells[kGapOffset] == 0xAA);   // gaps are never written

    if (argc > 1) {
        std::printf("{\"rva\":%u,\"body_length\":%u,\"base\":%llu,\"bytes\":\"",
                    0xd8fa40u, 93u, static_cast<unsigned long long>(0x3352bb0ull));
        for (auto byte : g.cells) std::printf("%02x", byte);
        std::printf("\"}\n");
    } else {
        std::puts("avx_d8fa40: all checks passed");
    }
    return 0;
}
