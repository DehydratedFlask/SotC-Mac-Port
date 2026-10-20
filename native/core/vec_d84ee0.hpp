// Native recovery of original ctor RVA 0xd84ee0 (index 84; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-73721c8d). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x1308450 into
// BSS @0x3352a70. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD84ee0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecD84ee0 = std::array<std::uint32_t, 4>;

void init_vec_d84ee0(VecD84ee0& g) noexcept;

}  // namespace sotc
