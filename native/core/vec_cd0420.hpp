// Native recovery of original ctor RVA 0xcd0420 (index 159; vector-copy manifest position 101;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-03. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f6a20 into BSS @0x3351c50. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCd0420Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCd0420 = std::array<std::uint32_t, 4>;

void init_vec_cd0420(VecCd0420& g) noexcept;

}  // namespace sotc
