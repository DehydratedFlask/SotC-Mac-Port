// Native recovery of original ctor RVA 0xd405a0 (index 110; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x1302220 into
// BSS @0x33520c0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD405a0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x33520c0.
using VecD405a0 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd405a0: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d405a0(VecD405a0& g) noexcept;

}  // namespace sotc
