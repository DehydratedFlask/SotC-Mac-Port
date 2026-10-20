// Native recovery of original ctor RVA 0xdb3960 (index 76; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x130d050 into
// BSS @0x3352c40. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecDb3960Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352c40.
using VecDb3960 = std::array<std::uint32_t, 4>;

// Behaviour of 0xdb3960: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_db3960(VecDb3960& g) noexcept;

}  // namespace sotc
