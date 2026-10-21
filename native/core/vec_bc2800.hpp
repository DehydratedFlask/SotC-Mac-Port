// Native recovery of original ctor RVA 0xbc2800 (index 257; vector-copy manifest position 189;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-23. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12deef0 into BSS @0x3350fd0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBC2800Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBC2800 = std::array<std::uint32_t, 4>;

void init_vec_bc2800(VecBC2800& g) noexcept;

}  // namespace sotc
