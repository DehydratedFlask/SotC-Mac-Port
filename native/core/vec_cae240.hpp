// Native recovery of original ctor RVA 0xcae240 (index 170; vector-copy manifest position 112;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-07. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f4270 into BSS @0x3351b90. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCae240Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCae240 = std::array<std::uint32_t, 4>;

void init_vec_cae240(VecCae240& g) noexcept;

}  // namespace sotc
