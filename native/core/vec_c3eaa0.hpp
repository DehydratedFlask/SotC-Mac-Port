// Native recovery of original ctor RVA 0xc3eaa0 (index 215; vector-copy manifest position 150;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-16. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ecc40 into BSS @0x3351600. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC3EAA0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC3EAA0 = std::array<std::uint32_t, 4>;

void init_vec_c3eaa0(VecC3EAA0& g) noexcept;

}  // namespace sotc
