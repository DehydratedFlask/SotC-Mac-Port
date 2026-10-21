// Native recovery of original ctor RVA 0xbd3190 (index 247; vector-copy manifest position 179;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-22. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e12c0 into BSS @0x3351170. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBD3190Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBD3190 = std::array<std::uint32_t, 4>;

void init_vec_bd3190(VecBD3190& g) noexcept;

}  // namespace sotc
