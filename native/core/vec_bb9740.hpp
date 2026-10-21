// Native recovery of original ctor RVA 0xbb9740 (index 263; vector-copy manifest position 195;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-24. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12dde70 into BSS @0x3350f20. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBB9740Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBB9740 = std::array<std::uint32_t, 4>;

void init_vec_bb9740(VecBB9740& g) noexcept;

}  // namespace sotc
