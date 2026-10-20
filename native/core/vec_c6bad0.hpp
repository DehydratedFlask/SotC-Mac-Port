// Native recovery of original ctor RVA 0xc6bad0 (index 190; vector-copy manifest position 126;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-11. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f0c80 into BSS @0x3351810. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC6bad0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC6bad0 = std::array<std::uint32_t, 4>;

void init_vec_c6bad0(VecC6bad0& g) noexcept;

}  // namespace sotc
