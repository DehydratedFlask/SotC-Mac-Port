// Native recovery of original ctor RVA 0xc686e0 (index 192; vector-copy manifest position 128;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-12. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f0720 into BSS @0x33517f0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC686e0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC686e0 = std::array<std::uint32_t, 4>;

void init_vec_c686e0(VecC686e0& g) noexcept;

}  // namespace sotc
