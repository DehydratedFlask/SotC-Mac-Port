// Native recovery of original ctor RVA 0xd47270 (index 108; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-73721c8d). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x1302a20 into
// BSS @0x33520f0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD47270Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecD47270 = std::array<std::uint32_t, 4>;

void init_vec_d47270(VecD47270& g) noexcept;

}  // namespace sotc
