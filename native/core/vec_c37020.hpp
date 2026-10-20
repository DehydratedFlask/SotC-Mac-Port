// Native recovery of original ctor RVA 0xc37020 (index 217; vector-copy manifest position 152;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-17. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ec290 into BSS @0x33515e0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC37020Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC37020 = std::array<std::uint32_t, 4>;

void init_vec_c37020(VecC37020& g) noexcept;

}  // namespace sotc
