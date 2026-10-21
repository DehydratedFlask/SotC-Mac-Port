// Native recovery of original ctor RVA 0xc675f0 (ctor index 193; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (115 bytes, 19 instructions):
// 9 straight-line stores into BSS cells 0x3351760..0x33517f0, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct LeafC675f0 {
    // Original cells 0x3351760..0x33517f0 (144 bytes, fully covered).
    std::array<std::uint8_t, 144> cells;
};

// Behaviour of 0xc675f0: rewrite every covered cell with the constants the original stores.
void init_leaf_c675f0(LeafC675f0& g) noexcept;

}  // namespace sotc
