// Native recovery of original ctor RVA 0xd853a0 (index 83; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x130a720 into
// BSS @0x3352a80. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD853a0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352a80.
using VecD853a0 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd853a0: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d853a0(VecD853a0& g) noexcept;

}  // namespace sotc
