// Native recovery of original ctor RVA 0xb52110 (index 290; ELF sha256 d0723afb...5861d3),
// task CPU-01d (claim-2b). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12d6b20 into
// BSS @0x3350ce0. Same bit pattern as 0xf30460 / XyzMaskCopy, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecB52110Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3350ce0.
using VecB52110 = std::array<std::uint32_t, 4>;

// Behaviour of 0xb52110: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_b52110(VecB52110& g) noexcept;

}  // namespace sotc
