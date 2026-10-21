// Native recovery of original ctor RVA 0x8a0440 (ctor index 321; ELF sha256
// d0723afb...5861d3). Unguarded, call-free constructor body (49 bytes, 7 instructions):
// 3 straight-line stores into BSS cells 0x3294f40..0x3294f70, no guards.
// Names are ours; the original has no symbols. Cell meanings are not analysed.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct Leaf8a0440 {
    // Original cells 0x3294f40..0x3294f70 (48 bytes, fully covered).
    std::array<std::uint8_t, 48> cells;
};

// Behaviour of 0x8a0440: rewrite every covered cell with the constants the original stores.
void init_leaf_8a0440(Leaf8a0440& g) noexcept;

}  // namespace sotc
