// Native recovery of original ctor RVA 0x83d800 (ctor index 324; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (61 bytes, 7 instructions):
// 6 straight-line 32-bit stores of 0 / 0xffffffff into BSS cells 0x3293400..0x329351c, no guards.
// The cells recur every 0x58 / 0x60 bytes (0x3293400, 0x3293458, 0x3293460, 0x32934b8, 0x32934c0,
// 0x3293518); the 260 bytes between the written dwords are never written.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct Leaf83d800 {
    // Original cells 0x3293400..0x329351c (284 bytes; only six dwords are covered).
    std::array<std::uint8_t, 284> cells;
};

// Behaviour of 0x83d800: rewrite the six covered dwords with the constants the original stores
// (0, 0xffffffff, 0, 0xffffffff, 0, 0xffffffff at cell offsets 0x000/0x058/0x060/0x0b8/0x0c0/0x118).
void init_leaf_83d800(Leaf83d800& g) noexcept;

}  // namespace sotc
