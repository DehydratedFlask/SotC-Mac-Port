// Native recovery of original ctor RVA 0xb747f0 (index 281; ELF sha256 d0723afb...5861d3),
// task CPU-01d. Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12d9100 into
// BSS @0x3350dc0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecB747f0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3350dc0.
using VecB747f0 = std::array<std::uint32_t, 4>;

// Behaviour of 0xb747f0: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_b747f0(VecB747f0& g) noexcept;

}  // namespace sotc
