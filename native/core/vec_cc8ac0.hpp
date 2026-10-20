// Native recovery of original ctor RVA 0xcc8ac0 (index 162; vector-copy manifest position 104;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-04. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f61d0 into BSS @0x3351c20. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCc8ac0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCc8ac0 = std::array<std::uint32_t, 4>;

void init_vec_cc8ac0(VecCc8ac0& g) noexcept;

}  // namespace sotc
