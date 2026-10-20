// Native recovery of original ctor RVA 0xce6130 (index 152; vector-copy manifest position 94;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-NEXT-01. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f8560 into BSS @0x3351cc0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCe6130Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCe6130 = std::array<std::uint32_t, 4>;

void init_vec_ce6130(VecCe6130& g) noexcept;

}  // namespace sotc
