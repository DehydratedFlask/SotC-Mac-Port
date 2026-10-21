// Native recovery of original ctor RVA 0xa816d0 (ctor index 304; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (47 bytes, 7 instructions):
// 5 straight-line stores into BSS cells 0x32fb1c0..0x32fb220, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct LeafA816d0 {
    // Original cells 0x32fb1c0..0x32fb220 (96 bytes, bytes no store covers are never written).
    std::array<std::uint8_t, 96> cells;
};

// Behaviour of 0xa816d0: rewrite every covered cell with the constants the original stores.
void init_leaf_a816d0(LeafA816d0& g) noexcept;

}  // namespace sotc
