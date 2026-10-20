// Native recovery of original ctor RVA 0xceb070 (ctor-survey.json index 87; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-effcf5dc). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12f95c0 into
// BSS @0x3351d30. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCeb070Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCeb070 = std::array<std::uint32_t, 4>;

void init_vec_ceb070(VecCeb070& g) noexcept;

}  // namespace sotc
