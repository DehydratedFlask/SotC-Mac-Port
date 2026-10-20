// Native recovery of original ctor RVA 0xdfd5f0 (index 54; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-73721c8d). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x13124a0 into
// BSS @0x3353340. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecDfd5f0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3353340.
using VecDfd5f0 = std::array<std::uint32_t, 4>;

// Behaviour of 0xdfd5f0: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_dfd5f0(VecDfd5f0& g) noexcept;

}  // namespace sotc
