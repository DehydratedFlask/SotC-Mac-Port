// Native recovery of original ctor RVA 0xcee8a0 (index 137; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-02397584). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12f9ec0 into
// BSS @0x3351d70. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCee8a0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCee8a0 = std::array<std::uint32_t, 4>;

void init_vec_cee8a0(VecCee8a0& g) noexcept;

}  // namespace sotc
