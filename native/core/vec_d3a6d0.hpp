// Native recovery of original ctor RVA 0xd3a6d0 (index 113; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x13016d0 into
// BSS @0x3352090. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD3a6d0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352090.
using VecD3a6d0 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd3a6d0: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d3a6d0(VecD3a6d0& g) noexcept;

}  // namespace sotc
