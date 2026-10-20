// Native recovery of original ctor RVA 0xd08240 (ctor-survey.json index 76; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-effcf5dc). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12fce70 into
// BSS @0x3351de0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD08240Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecD08240 = std::array<std::uint32_t, 4>;

void init_vec_d08240(VecD08240& g) noexcept;

}  // namespace sotc
