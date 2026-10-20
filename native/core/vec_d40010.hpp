// Native recovery of original ctor RVA 0xd40010 (index 112; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-73721c8d). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x1301e30 into
// BSS @0x33520b0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD40010Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecD40010 = std::array<std::uint32_t, 4>;

void init_vec_d40010(VecD40010& g) noexcept;

}  // namespace sotc
