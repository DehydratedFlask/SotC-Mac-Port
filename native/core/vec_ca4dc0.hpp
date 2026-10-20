// Native recovery of original ctor RVA 0xca4dc0 (index 173; vector-copy manifest position 114;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-08. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f3ac0 into BSS @0x3351b10. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCa4dc0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCa4dc0 = std::array<std::uint32_t, 4>;

void init_vec_ca4dc0(VecCa4dc0& g) noexcept;

}  // namespace sotc
