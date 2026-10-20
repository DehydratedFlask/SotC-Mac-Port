// Native recovery of original ctor RVA 0xd2cb30 (index 121; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-73721c8d). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x13007c0 into
// BSS @0x3351eb0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD2cb30Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecD2cb30 = std::array<std::uint32_t, 4>;

void init_vec_d2cb30(VecD2cb30& g) noexcept;

}  // namespace sotc
