// Native recovery of original ctor RVA 0xbd60a0 (index 244; vector-copy manifest position 176;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-21. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e1a30 into BSS @0x33511a0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBD60A0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBD60A0 = std::array<std::uint32_t, 4>;

void init_vec_bd60a0(VecBD60A0& g) noexcept;

}  // namespace sotc
