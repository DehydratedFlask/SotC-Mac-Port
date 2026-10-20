// Native recovery of original ctor RVA 0xc786c0 (index 182; vector-copy manifest position 120;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-10. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f1ee0 into BSS @0x33519d0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC786c0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC786c0 = std::array<std::uint32_t, 4>;

void init_vec_c786c0(VecC786c0& g) noexcept;

}  // namespace sotc
