// Native recovery of original ctor RVA 0xe0b180 (survey index 49; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-71956f82). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x13141b0 into
// BSS @0x3353390. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecE0b180Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3353390.
using VecE0b180 = std::array<std::uint32_t, 4>;

// Behaviour of 0xe0b180: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_e0b180(VecE0b180& g) noexcept;

}  // namespace sotc
