// Native recovery of original ctor RVA 0xd09160 (ctor-survey.json index 74; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-effcf5dc). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12fd200 into
// BSS @0x3351e00. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD09160Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecD09160 = std::array<std::uint32_t, 4>;

void init_vec_d09160(VecD09160& g) noexcept;

}  // namespace sotc
