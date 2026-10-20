// Native recovery of original ctor RVA 0xcceaa0 (index 160; vector-copy manifest position 102;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-04. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f67d0 into BSS @0x3351c40. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCceaa0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCceaa0 = std::array<std::uint32_t, 4>;

void init_vec_cceaa0(VecCceaa0& g) noexcept;

}  // namespace sotc
