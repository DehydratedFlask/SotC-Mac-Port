// Native recovery of original ctor RVA 0xc4aa80 (index 210; vector-copy manifest position 145;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-15. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12edcc0 into BSS @0x3351650. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC4AA80Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC4AA80 = std::array<std::uint32_t, 4>;

void init_vec_c4aa80(VecC4AA80& g) noexcept;

}  // namespace sotc
