// Native recovery of original ctor RVA 0x3ca7c0 (ctor index 466; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (38 bytes, 8 instructions):
// 3 straight-line stores into BSS cells 0x2ef6f88..0x2ef6fe8, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct Leaf3ca7c0 {
    // Original cells 0x2ef6f88..0x2ef6fe8 (96 bytes, fully covered).
    std::array<std::uint8_t, 96> cells;
};

// Behaviour of 0x3ca7c0: rewrite every covered cell with the constants the original stores.
void init_leaf_3ca7c0(Leaf3ca7c0& g) noexcept;

}  // namespace sotc
