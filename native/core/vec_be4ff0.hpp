// Native recovery of original ctor RVA 0xbe4ff0 (index 241; vector-copy manifest position 173;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-20. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e2490 into BSS @0x33511d0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBE4FF0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBE4FF0 = std::array<std::uint32_t, 4>;

void init_vec_be4ff0(VecBE4FF0& g) noexcept;

}  // namespace sotc
