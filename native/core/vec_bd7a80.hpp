// Native recovery of original ctor RVA 0xbd7a80 (index 242; vector-copy manifest position 174;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-20. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e2290 into BSS @0x33511c0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBD7A80Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBD7A80 = std::array<std::uint32_t, 4>;

void init_vec_bd7a80(VecBD7A80& g) noexcept;

}  // namespace sotc
