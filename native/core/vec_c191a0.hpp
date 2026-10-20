// Native recovery of original ctor RVA 0xc191a0 (index 223; vector-copy manifest position 158;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-18. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e97a0 into BSS @0x3351540. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC191A0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC191A0 = std::array<std::uint32_t, 4>;

void init_vec_c191a0(VecC191A0& g) noexcept;

}  // namespace sotc
