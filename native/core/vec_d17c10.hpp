// Native recovery of original ctor RVA 0xd17c10 (index 127; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-9aa0fe88). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12fe4b0 into
// BSS @0x3351e50. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD17c10Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecD17c10 = std::array<std::uint32_t, 4>;

void init_vec_d17c10(VecD17c10& g) noexcept;

}  // namespace sotc
