// Native recovery of original ctor RVA 0xbbf580 (index 260; vector-copy manifest position 192;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-24. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12de530 into BSS @0x3350f50. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBBF580Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBBF580 = std::array<std::uint32_t, 4>;

void init_vec_bbf580(VecBBF580& g) noexcept;

}  // namespace sotc
