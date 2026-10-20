// Native recovery of original ctor RVA 0xc5c1a0 (index 201; vector-copy manifest position 136;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-13. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ef200 into BSS @0x33516e0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC5c1a0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC5c1a0 = std::array<std::uint32_t, 4>;

void init_vec_c5c1a0(VecC5c1a0& g) noexcept;

}  // namespace sotc
