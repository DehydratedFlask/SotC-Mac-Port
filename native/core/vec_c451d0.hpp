// Native recovery of original ctor RVA 0xc451d0 (index 212; vector-copy manifest position 147;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-16. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ed6e0 into BSS @0x3351630. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC451D0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC451D0 = std::array<std::uint32_t, 4>;

void init_vec_c451d0(VecC451D0& g) noexcept;

}  // namespace sotc
