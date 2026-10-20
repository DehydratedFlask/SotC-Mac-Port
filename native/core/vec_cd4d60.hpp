// Native recovery of original ctor RVA 0xcd4d60 (index 158; vector-copy manifest position 100;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-03. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f6e70 into BSS @0x3351c60. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCd4d60Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCd4d60 = std::array<std::uint32_t, 4>;

void init_vec_cd4d60(VecCd4d60& g) noexcept;

}  // namespace sotc
