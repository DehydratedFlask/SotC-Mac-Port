// Native recovery of original ctor RVA 0xc0bea0 (index 230; vector-copy manifest position 163;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-19. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e7b50 into BSS @0x3351310. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC0BEA0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC0BEA0 = std::array<std::uint32_t, 4>;

void init_vec_c0bea0(VecC0BEA0& g) noexcept;

}  // namespace sotc
