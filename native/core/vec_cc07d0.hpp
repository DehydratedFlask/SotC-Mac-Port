// Native recovery of original ctor RVA 0xcc07d0 (index 165; vector-copy manifest position 107;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-05. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f57d0 into BSS @0x3351bf0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCc07d0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCc07d0 = std::array<std::uint32_t, 4>;

void init_vec_cc07d0(VecCc07d0& g) noexcept;

}  // namespace sotc
