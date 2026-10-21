// Native recovery of original ctor RVA 0xbbba10 (index 262; vector-copy manifest position 194;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-24. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12de090 into BSS @0x3350f30. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBBBA10Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBBBA10 = std::array<std::uint32_t, 4>;

void init_vec_bbba10(VecBBBA10& g) noexcept;

}  // namespace sotc
