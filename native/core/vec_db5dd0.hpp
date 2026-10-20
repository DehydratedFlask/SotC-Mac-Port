// Native recovery of original ctor RVA 0xdb5dd0 (index 74; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-73721c8d). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x130d380 into
// BSS @0x3352c60. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecDb5dd0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecDb5dd0 = std::array<std::uint32_t, 4>;

void init_vec_db5dd0(VecDb5dd0& g) noexcept;

}  // namespace sotc
