// Native recovery of original ctor RVA 0xbe50a0 (index 240; vector-copy manifest position 172;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-20. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e3290 into BSS @0x33511e0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBE50A0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBE50A0 = std::array<std::uint32_t, 4>;

void init_vec_be50a0(VecBE50A0& g) noexcept;

}  // namespace sotc
