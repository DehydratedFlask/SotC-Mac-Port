// Native recovery of original ctor RVA 0xc70e20 (index 185; vector-copy manifest position 123;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-10. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f16a0 into BSS @0x33519a0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC70e20Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC70e20 = std::array<std::uint32_t, 4>;

void init_vec_c70e20(VecC70e20& g) noexcept;

}  // namespace sotc
