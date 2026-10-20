// Native recovery of original ctor RVA 0xc47e00 (index 211; vector-copy manifest position 146;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-15. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ed9a0 into BSS @0x3351640. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC47E00Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC47E00 = std::array<std::uint32_t, 4>;

void init_vec_c47e00(VecC47E00& g) noexcept;

}  // namespace sotc
