// Native recovery of original ctor RVA 0xc44830 (index 213; vector-copy manifest position 148;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-16. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ed4f0 into BSS @0x3351620. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC44830Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC44830 = std::array<std::uint32_t, 4>;

void init_vec_c44830(VecC44830& g) noexcept;

}  // namespace sotc
