// Native recovery of original ctor RVA 0xccd0f0 (index 161; vector-copy manifest position 103;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-04. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f6560 into BSS @0x3351c30. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCcd0f0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCcd0f0 = std::array<std::uint32_t, 4>;

void init_vec_ccd0f0(VecCcd0f0& g) noexcept;

}  // namespace sotc
