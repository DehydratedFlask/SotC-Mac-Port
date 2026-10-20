// Native recovery of original ctor RVA 0xc22300 (index 220; vector-copy manifest position 155;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-17. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ea490 into BSS @0x3351570. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC22300Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC22300 = std::array<std::uint32_t, 4>;

void init_vec_c22300(VecC22300& g) noexcept;

}  // namespace sotc
