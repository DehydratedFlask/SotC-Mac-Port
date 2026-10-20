// Native recovery of original ctor RVA 0xce99c0 (index 146; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12f9370 into
// BSS @0x3351d20. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCe99c0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCe99c0 = std::array<std::uint32_t, 4>;

void init_vec_ce99c0(VecCe99c0& g) noexcept;

}  // namespace sotc
