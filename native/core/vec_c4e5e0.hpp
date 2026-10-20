// Native recovery of original ctor RVA 0xc4e5e0 (index 209; vector-copy manifest position 144;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-15. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12edf70 into BSS @0x3351660. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC4E5E0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC4E5E0 = std::array<std::uint32_t, 4>;

void init_vec_c4e5e0(VecC4E5E0& g) noexcept;

}  // namespace sotc
