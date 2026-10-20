// Native recovery of original ctor RVA 0xb743c0 (index 282; ELF sha256 d0723afb...5861d3),
// task CPU-01d (claim-2c). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12d8d90 into
// BSS @0x3350db0. Same bit pattern as 0xf30460 / 0xb52110, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecB743c0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3350db0.
using VecB743c0 = std::array<std::uint32_t, 4>;

// Behaviour of 0xb743c0: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_b743c0(VecB743c0& g) noexcept;

}  // namespace sotc
