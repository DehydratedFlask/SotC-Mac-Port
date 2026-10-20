// Native recovery of original ctor RVA 0xc77070 (index 183; vector-copy manifest position 121;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-10. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f1c00 into BSS @0x33519c0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC77070Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC77070 = std::array<std::uint32_t, 4>;

void init_vec_c77070(VecC77070& g) noexcept;

}  // namespace sotc
