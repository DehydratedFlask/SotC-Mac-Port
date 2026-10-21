// Native recovery of original ctor RVA 0xc6f520 (ctor index 187; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (66 bytes, 9 instructions):
// 4 straight-line stores into BSS cells 0x3351930..0x3351988, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct LeafC6f520 {
    // Original cells 0x3351930..0x3351988 (88 bytes, fully covered).
    std::array<std::uint8_t, 88> cells;
};

// Behaviour of 0xc6f520: rewrite every covered cell with the constants the original stores.
void init_leaf_c6f520(LeafC6f520& g) noexcept;

}  // namespace sotc
