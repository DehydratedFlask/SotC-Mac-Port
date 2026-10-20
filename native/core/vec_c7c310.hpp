// Native recovery of original ctor RVA 0xc7c310 (index 181; vector-copy manifest position 119;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-09. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f2250 into BSS @0x33519e0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC7c310Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC7c310 = std::array<std::uint32_t, 4>;

void init_vec_c7c310(VecC7c310& g) noexcept;

}  // namespace sotc
