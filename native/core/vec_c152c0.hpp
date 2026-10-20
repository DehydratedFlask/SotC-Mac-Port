// Native recovery of original ctor RVA 0xc152c0 (index 226; vector-copy manifest position 160;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-18. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e8c90 into BSS @0x33513e0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC152C0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC152C0 = std::array<std::uint32_t, 4>;

void init_vec_c152c0(VecC152C0& g) noexcept;

}  // namespace sotc
