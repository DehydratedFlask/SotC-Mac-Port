// Native recovery of original ctor RVA 0xdc2d50 (survey index 69; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-effcf5dc). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x130e010 into
// BSS @0x3352cb0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecDc2d50Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352cb0.
using VecDc2d50 = std::array<std::uint32_t, 4>;

// Behaviour of 0xdc2d50: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_dc2d50(VecDc2d50& g) noexcept;

}  // namespace sotc
