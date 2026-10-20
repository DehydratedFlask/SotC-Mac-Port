// Native recovery of original ctor RVA 0xc6fdf0 (index 186; vector-copy manifest position 124;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-11. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f14e0 into BSS @0x3351990. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC6fdf0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC6fdf0 = std::array<std::uint32_t, 4>;

void init_vec_c6fdf0(VecC6fdf0& g) noexcept;

}  // namespace sotc
