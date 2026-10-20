// Native recovery of original ctor RVA 0xcad450 (index 171; vector-copy manifest position 113;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-07. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f4010 into BSS @0x3351b80. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCad450Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCad450 = std::array<std::uint32_t, 4>;

void init_vec_cad450(VecCad450& g) noexcept;

}  // namespace sotc
