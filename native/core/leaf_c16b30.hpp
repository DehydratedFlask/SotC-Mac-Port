// Native recovery of original ctor RVA 0xc16b30 (ctor index 225; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (186 bytes, 26 instructions):
// 19 straight-line stores into BSS cells 0x33513f0..0x3351530, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct LeafC16b30 {
    // Original cells 0x33513f0..0x3351530 (320 bytes, fully covered).
    std::array<std::uint8_t, 320> cells;
};

// Behaviour of 0xc16b30: rewrite every covered cell with the constants the original stores.
void init_leaf_c16b30(LeafC16b30& g) noexcept;

}  // namespace sotc
