// Native recovery of original ctor RVA 0xd2c7e0 (index 122; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-9aa0fe88). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12ff9e0 into
// BSS @0x3351ea0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD2c7e0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecD2c7e0 = std::array<std::uint32_t, 4>;

void init_vec_d2c7e0(VecD2c7e0& g) noexcept;

}  // namespace sotc
