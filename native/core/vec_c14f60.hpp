// Native recovery of original ctor RVA 0xc14f60 (index 227; vector-copy manifest position 161;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-18. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e84f0 into BSS @0x33513d0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC14F60Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC14F60 = std::array<std::uint32_t, 4>;

void init_vec_c14f60(VecC14F60& g) noexcept;

}  // namespace sotc
