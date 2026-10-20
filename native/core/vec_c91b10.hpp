// Native recovery of original ctor RVA 0xc91b10 (index 177; vector-copy manifest position 116;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-08. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f2fd0 into BSS @0x3351a30. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC91b10Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC91b10 = std::array<std::uint32_t, 4>;

void init_vec_c91b10(VecC91b10& g) noexcept;

}  // namespace sotc
