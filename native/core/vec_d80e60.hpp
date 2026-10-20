// Native recovery of original ctor RVA 0xd80e60 (index 85; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x1307f70 into
// BSS @0x3352a60. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD80e60Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352a60.
using VecD80e60 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd80e60: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d80e60(VecD80e60& g) noexcept;

}  // namespace sotc
