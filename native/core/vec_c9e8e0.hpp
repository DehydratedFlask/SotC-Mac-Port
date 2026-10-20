// Native recovery of original ctor RVA 0xc9e8e0 (index 175; vector-copy manifest position 115;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-08. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f3550 into BSS @0x3351aa0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC9e8e0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC9e8e0 = std::array<std::uint32_t, 4>;

void init_vec_c9e8e0(VecC9e8e0& g) noexcept;

}  // namespace sotc
