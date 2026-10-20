// Native recovery of original ctor RVA 0xdc4180 (index 68; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-73721c8d). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x130e220 into
// BSS @0x3352cc0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecDc4180Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352cc0.
using VecDc4180 = std::array<std::uint32_t, 4>;

// Behaviour of 0xdc4180: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_dc4180(VecDc4180& g) noexcept;

}  // namespace sotc
