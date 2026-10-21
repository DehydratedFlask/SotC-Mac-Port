// Native recovery of original ctor RVA 0xb692a0 (ctor index 285; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (93 bytes, 13 instructions):
// 6 straight-line stores into BSS cells 0x3350d30..0x3350d90, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct LeafB692a0 {
    // Original cells 0x3350d30..0x3350d90 (96 bytes, fully covered).
    std::array<std::uint8_t, 96> cells;
};

// Behaviour of 0xb692a0: rewrite every covered cell with the constants the original stores.
void init_leaf_b692a0(LeafB692a0& g) noexcept;

}  // namespace sotc
