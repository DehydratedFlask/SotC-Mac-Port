// Native recovery of original ctor RVA 0xcab080 (ctor index 172; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (93 bytes, 13 instructions):
// 6 straight-line stores into BSS cells 0x3351b20..0x3351b80, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct LeafCab080 {
    // Original cells 0x3351b20..0x3351b80 (96 bytes, fully covered).
    std::array<std::uint8_t, 96> cells;
};

// Behaviour of 0xcab080: rewrite every covered cell with the constants the original stores.
void init_leaf_cab080(LeafCab080& g) noexcept;

}  // namespace sotc
