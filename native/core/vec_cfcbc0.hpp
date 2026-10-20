// Native recovery of original ctor RVA 0xcfcbc0 (index 139; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12fa850 into
// BSS @0x3351d90. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCfcbc0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCfcbc0 = std::array<std::uint32_t, 4>;

void init_vec_cfcbc0(VecCfcbc0& g) noexcept;

}  // namespace sotc
