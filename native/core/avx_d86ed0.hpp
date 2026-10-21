// Native recovery of original ctor RVA 0xd86ed0 (ctor index 81; ELF sha256
// d0723afb...5861d3). Unguarded constructor: a chain of AVX moves that publishes rodata
// constants into one contiguous run of BSS cells, with no guards and no calls.
// Names are ours; the original has no symbols.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct AvxD86ed0 {
    // Original cells 0x3352aa0..0x3352bb0 (272 bytes, gaps are BSS zero).
    std::array<std::uint8_t, 272> cells;
};

// Behaviour of 0xd86ed0: rewrite every cell with the constants the original stores.
void init_avx_d86ed0(AvxD86ed0& g) noexcept;

}  // namespace sotc
