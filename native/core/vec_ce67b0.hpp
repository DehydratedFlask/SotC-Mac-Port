// Native recovery of original ctor RVA 0xce67b0 (index 151; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-73721c8d). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12f8790 into
// BSS @0x3351cd0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCe67b0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCe67b0 = std::array<std::uint32_t, 4>;

void init_vec_ce67b0(VecCe67b0& g) noexcept;

}  // namespace sotc
