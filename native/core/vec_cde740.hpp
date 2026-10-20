// Native recovery of original ctor RVA 0xcde740 (index 156; vector-copy manifest position 98;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-03. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f77e0 into BSS @0x3351c80. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCde740Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCde740 = std::array<std::uint32_t, 4>;

void init_vec_cde740(VecCde740& g) noexcept;

}  // namespace sotc
