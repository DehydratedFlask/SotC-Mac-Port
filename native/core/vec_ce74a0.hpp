// Native recovery of original ctor RVA 0xce74a0 (index 148; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12f8d90 into
// BSS @0x3351d00. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCe74a0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCe74a0 = std::array<std::uint32_t, 4>;

void init_vec_ce74a0(VecCe74a0& g) noexcept;

}  // namespace sotc
