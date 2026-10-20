// Native recovery of original ctor RVA 0xd7fe90 (index 86; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x1307dc0 into
// BSS @0x3352a50. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD7fe90Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352a50.
using VecD7fe90 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd7fe90: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d7fe90(VecD7fe90& g) noexcept;

}  // namespace sotc
