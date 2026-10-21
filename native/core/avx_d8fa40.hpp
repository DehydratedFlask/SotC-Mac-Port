// Native recovery of original ctor RVA 0xd8fa40 (ctor index 80; ELF sha256
// d0723afb...5861d3). Unguarded constructor: a chain of AVX moves that publishes rodata
// constants into one contiguous run of BSS cells, with no guards and no calls.
// The first cell is the {~0,~0,~0,0} lane mask; the rest are float splats
// (2.0f, -0.0f, 1e-8f, 1e-9f, 0.0f by IEEE value).
// Names are ours; the original has no symbols.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct AvxD8fa40 {
    // Original cells 0x3352bb0..0x3352c10 (96 bytes, gaps are BSS zero).
    std::array<std::uint8_t, 96> cells;
};

// Behaviour of 0xd8fa40: rewrite every cell with the constants the original stores.
void init_avx_d8fa40(AvxD8fa40& g) noexcept;

}  // namespace sotc
