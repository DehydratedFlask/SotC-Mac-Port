// Native recovery of original ctor RVA 0xd00600 (index 137; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12fb8a0 into
// BSS @0x3351db0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD00600Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecD00600 = std::array<std::uint32_t, 4>;

void init_vec_d00600(VecD00600& g) noexcept;

}  // namespace sotc
