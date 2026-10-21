// Native recovery of original ctor RVA 0xc6e9d0 (ctor index 188; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (149 bytes, 19 instructions):
// 10 straight-line stores into BSS cells 0x3351830..0x3351930, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct LeafC6e9d0 {
    // Original cells 0x3351830..0x3351930 (256 bytes, bytes no store covers are never written).
    std::array<std::uint8_t, 256> cells;
};

// Behaviour of 0xc6e9d0: rewrite every covered cell with the constants the original stores.
void init_leaf_c6e9d0(LeafC6e9d0& g) noexcept;

}  // namespace sotc
