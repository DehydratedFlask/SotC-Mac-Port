// Native recovery of original ctor RVA 0xbcc3f0 (index 253; vector-copy manifest position 185;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-23. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12dfe60 into BSS @0x3351110. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBCC3F0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBCC3F0 = std::array<std::uint32_t, 4>;

void init_vec_bcc3f0(VecBCC3F0& g) noexcept;

}  // namespace sotc
