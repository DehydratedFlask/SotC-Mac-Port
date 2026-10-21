// Native recovery of original ctor RVA 0x163a40 (ctor index 526; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (129 bytes, 16 instructions):
// 10 straight-line stores into BSS cells 0x2ee3c60..0x2ee3cd8, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct Leaf163a40 {
    // Original cells 0x2ee3c60..0x2ee3cd8 (120 bytes, bytes no store covers are never written).
    std::array<std::uint8_t, 120> cells;
};

// Behaviour of 0x163a40: rewrite every covered cell with the constants the original stores.
void init_leaf_163a40(Leaf163a40& g) noexcept;

}  // namespace sotc
