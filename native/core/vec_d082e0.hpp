// Native recovery of original ctor RVA 0xd082e0 (index 133; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12fd050 into
// BSS @0x3351df0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD082e0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3351df0.
using VecD082e0 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd082e0: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d082e0(VecD082e0& g) noexcept;

}  // namespace sotc
