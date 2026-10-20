// Native recovery of original ctor RVA 0xd3b790 (index 112; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x1301a50 into
// BSS @0x33520a0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD3b790Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x33520a0.
using VecD3b790 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd3b790: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d3b790(VecD3b790& g) noexcept;

}  // namespace sotc
