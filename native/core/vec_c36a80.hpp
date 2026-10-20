// Native recovery of original ctor RVA 0xc36a80 (index 218; vector-copy manifest position 153;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-17. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12eaf90 into BSS @0x3351590. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC36A80Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC36A80 = std::array<std::uint32_t, 4>;

void init_vec_c36a80(VecC36A80& g) noexcept;

}  // namespace sotc
