// Native recovery of original ctor RVA 0xc555c0 (index 205; vector-copy manifest position 140;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-14. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ee940 into BSS @0x33516a0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC555C0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC555C0 = std::array<std::uint32_t, 4>;

void init_vec_c555c0(VecC555C0& g) noexcept;

}  // namespace sotc
