// Native recovery of original ctor RVA 0xef85e0 (index 20; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-dc141fdf). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x1324f80 into
// BSS @0x3354180. Same bit pattern as 0xf30460 / 0xb52110 / 0xb743c0, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecEf85e0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3354180.
using VecEf85e0 = std::array<std::uint32_t, 4>;

// Behaviour of 0xef85e0: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_ef85e0(VecEf85e0& g) noexcept;

}  // namespace sotc
