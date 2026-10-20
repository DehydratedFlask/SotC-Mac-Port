// Native recovery of original ctor RVA 0xd0a250 (index 131; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12fd3b0 into
// BSS @0x3351e10. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD0a250Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3351e10.
using VecD0a250 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd0a250: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d0a250(VecD0a250& g) noexcept;

}  // namespace sotc
