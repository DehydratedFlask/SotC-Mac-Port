// Native recovery of original ctor RVA 0xc0d380 (ctor index 229; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (92 bytes, 13 instructions):
// 8 straight-line stores into BSS cells 0x3351320..0x33513c0, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct LeafC0d380 {
    // Original cells 0x3351320..0x33513c0 (160 bytes, bytes no store covers are never written).
    std::array<std::uint8_t, 160> cells;
};

// Behaviour of 0xc0d380: rewrite every covered cell with the constants the original stores.
void init_leaf_c0d380(LeafC0d380& g) noexcept;

}  // namespace sotc
