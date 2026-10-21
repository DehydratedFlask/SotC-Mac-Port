// Native recovery of original ctor RVA 0xc05980 (ctor index 237; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (31 bytes, 5 instructions):
// 3 straight-line stores into BSS cells 0x3351290..0x33512a2, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct LeafC05980 {
    // Original cells 0x3351290..0x33512a2 (18 bytes, fully covered).
    std::array<std::uint8_t, 18> cells;
};

// Behaviour of 0xc05980: rewrite every covered cell with the constants the original stores.
void init_leaf_c05980(LeafC05980& g) noexcept;

}  // namespace sotc
