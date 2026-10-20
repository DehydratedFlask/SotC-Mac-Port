// Native recovery of original ctor RVA 0xceb2c0 (index 143; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-9aa0fe88). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12f9990 into
// BSS @0x3351d50. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCeb2c0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCeb2c0 = std::array<std::uint32_t, 4>;

void init_vec_ceb2c0(VecCeb2c0& g) noexcept;

}  // namespace sotc
