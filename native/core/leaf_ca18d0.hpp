// Native recovery of original ctor RVA 0xca18d0 (ctor index 174; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (93 bytes, 13 instructions):
// 6 straight-line stores into BSS cells 0x3351ab0..0x3351b10, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct LeafCa18d0 {
    // Original cells 0x3351ab0..0x3351b10 (96 bytes, fully covered).
    std::array<std::uint8_t, 96> cells;
};

// Behaviour of 0xca18d0: rewrite every covered cell with the constants the original stores.
void init_leaf_ca18d0(LeafCa18d0& g) noexcept;

}  // namespace sotc
