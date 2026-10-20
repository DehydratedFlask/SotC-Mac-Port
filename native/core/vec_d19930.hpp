// Native recovery of original ctor RVA 0xd19930 (index 126; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-73721c8d). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12fe8e0 into
// BSS @0x3351e60. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD19930Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecD19930 = std::array<std::uint32_t, 4>;

void init_vec_d19930(VecD19930& g) noexcept;

}  // namespace sotc
