// Native recovery of original ctor RVA 0xbd32f0 (index 246; vector-copy manifest position 178;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-22. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e14c0 into BSS @0x3351180. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBD32F0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBD32F0 = std::array<std::uint32_t, 4>;

void init_vec_bd32f0(VecBD32F0& g) noexcept;

}  // namespace sotc
