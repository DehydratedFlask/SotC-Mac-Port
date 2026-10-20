// Native recovery of original ctor RVA 0xce6fb0 (index 150; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12f89a0 into
// BSS @0x3351ce0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCe6fb0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCe6fb0 = std::array<std::uint32_t, 4>;

void init_vec_ce6fb0(VecCe6fb0& g) noexcept;

}  // namespace sotc
