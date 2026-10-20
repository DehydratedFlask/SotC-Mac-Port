// Native recovery of original ctor RVA 0xcfcc60 (index 138; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-9aa0fe88). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12fb6b0 into
// BSS @0x3351da0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCfcc60Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCfcc60 = std::array<std::uint32_t, 4>;

void init_vec_cfcc60(VecCfcc60& g) noexcept;

}  // namespace sotc
