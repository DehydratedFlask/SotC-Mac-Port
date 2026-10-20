// Native recovery of original ctor RVA 0xc5c040 (index 202; vector-copy manifest position 137;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-14. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ef040 into BSS @0x33516d0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC5C040Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC5C040 = std::array<std::uint32_t, 4>;

void init_vec_c5c040(VecC5C040& g) noexcept;

}  // namespace sotc
