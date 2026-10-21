// Native recovery of original ctor RVA 0xc811b0 (ctor index 179; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (33 bytes, 5 instructions):
// 2 straight-line stores into BSS cells 0x3351a00..0x3351a20, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct LeafC811b0 {
    // Original cells 0x3351a00..0x3351a20 (32 bytes, fully covered).
    std::array<std::uint8_t, 32> cells;
};

// Behaviour of 0xc811b0: rewrite every covered cell with the constants the original stores.
void init_leaf_c811b0(LeafC811b0& g) noexcept;

}  // namespace sotc
