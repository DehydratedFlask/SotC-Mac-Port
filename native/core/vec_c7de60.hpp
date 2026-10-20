// Native recovery of original ctor RVA 0xc7de60 (index 180; vector-copy manifest position 118;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-09. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f2520 into BSS @0x33519f0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC7de60Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC7de60 = std::array<std::uint32_t, 4>;

void init_vec_c7de60(VecC7de60& g) noexcept;

}  // namespace sotc
