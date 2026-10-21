// Native recovery of original ctor RVA 0xc94ae0 (ctor index 176; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (93 bytes, 13 instructions):
// 6 straight-line stores into BSS cells 0x3351a40..0x3351aa0, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct LeafC94ae0 {
    // Original cells 0x3351a40..0x3351aa0 (96 bytes, fully covered).
    std::array<std::uint8_t, 96> cells;
};

// Behaviour of 0xc94ae0: rewrite every covered cell with the constants the original stores.
void init_leaf_c94ae0(LeafC94ae0& g) noexcept;

}  // namespace sotc
