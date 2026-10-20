// Native recovery of original ctor RVA 0xdddc90 (index 61; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-73721c8d). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x130f350 into
// BSS @0x3352ec0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecDddc90Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352ec0.
using VecDddc90 = std::array<std::uint32_t, 4>;

// Behaviour of 0xdddc90: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_dddc90(VecDddc90& g) noexcept;

}  // namespace sotc
