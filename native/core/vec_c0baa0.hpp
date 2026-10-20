// Native recovery of original ctor RVA 0xc0baa0 (index 233; vector-copy manifest position 166;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-19. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e7520 into BSS @0x33512e0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC0BAA0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC0BAA0 = std::array<std::uint32_t, 4>;

void init_vec_c0baa0(VecC0BAA0& g) noexcept;

}  // namespace sotc
