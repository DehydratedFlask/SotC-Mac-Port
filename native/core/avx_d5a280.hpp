// Native recovery of original ctor RVA 0xd5a280 (ctor index 105; ELF sha256
// d0723afb...5861d3). Unguarded constructor with one loop: it publishes a 16-byte lane mask at
// 0x3352120 and then fills 0x800 bytes at 0x3352130 with an
// int32 identity table (0,1,2,...). The loop adds a 32-splat to eight int32 lanes per iteration;
// the decoded constants make the result table[i] == i, which the test checks independently.
// Names are ours; the original has no symbols.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct AvxD5a280 {
    std::array<std::uint8_t, 16> prefix;         // original cell 0x3352120
    std::array<std::uint8_t, 0x800> table;   // original cells 0x3352130..0x3352930
};

// Behaviour of 0xd5a280: write the mask, then the 512-entry int32 identity table.
void init_avx_d5a280(AvxD5a280& g) noexcept;

}  // namespace sotc
