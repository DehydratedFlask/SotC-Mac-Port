// Native recovery of original ctor RVA 0xc0bb00 (index 232; vector-copy manifest position 165;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-19. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e7830 into BSS @0x33512f0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC0BB00Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC0BB00 = std::array<std::uint32_t, 4>;

void init_vec_c0bb00(VecC0BB00& g) noexcept;

}  // namespace sotc
