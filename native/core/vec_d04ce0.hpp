// Native recovery of original ctor RVA 0xd04ce0 (index 136; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-73721c8d). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12fc350 into
// BSS @0x3351dc0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD04ce0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecD04ce0 = std::array<std::uint32_t, 4>;

void init_vec_d04ce0(VecD04ce0& g) noexcept;

}  // namespace sotc
