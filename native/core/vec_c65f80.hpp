// Native recovery of original ctor RVA 0xc65f80 (index 195; vector-copy manifest position 130;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-12. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12effa0 into BSS @0x3351740. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC65f80Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC65f80 = std::array<std::uint32_t, 4>;

void init_vec_c65f80(VecC65f80& g) noexcept;

}  // namespace sotc
