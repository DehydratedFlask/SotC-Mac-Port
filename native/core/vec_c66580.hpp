// Native recovery of original ctor RVA 0xc66580 (index 194; vector-copy manifest position 129;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-12. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f0160 into BSS @0x3351750. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC66580Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC66580 = std::array<std::uint32_t, 4>;

void init_vec_c66580(VecC66580& g) noexcept;

}  // namespace sotc
