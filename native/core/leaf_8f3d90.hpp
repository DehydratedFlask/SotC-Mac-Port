// Native recovery of original ctor RVA 0x8f3d90 (ctor index 320; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (257 bytes, 33 instructions):
// 16 straight-line stores into BSS cells 0x3294f90..0x3295090, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct Leaf8f3d90 {
    // Original cells 0x3294f90..0x3295090 (256 bytes, fully covered).
    std::array<std::uint8_t, 256> cells;
};

// Behaviour of 0x8f3d90: rewrite every covered cell with the constants the original stores.
void init_leaf_8f3d90(Leaf8f3d90& g) noexcept;

}  // namespace sotc
