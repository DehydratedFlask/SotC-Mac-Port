// Native recovery of original ctor RVA 0xc0d720 (index 228; vector-copy manifest position 162;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-18. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e8260 into BSS @0x33513c0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC0D720Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC0D720 = std::array<std::uint32_t, 4>;

void init_vec_c0d720(VecC0D720& g) noexcept;

}  // namespace sotc
