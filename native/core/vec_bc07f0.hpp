// Native recovery of original ctor RVA 0xbc07f0 (index 258; vector-copy manifest position 190;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-24. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ded00 into BSS @0x3350fc0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBC07F0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBC07F0 = std::array<std::uint32_t, 4>;

void init_vec_bc07f0(VecBC07F0& g) noexcept;

}  // namespace sotc
