// Native recovery of original ctor RVA 0xc5f6f0 (index 199; vector-copy manifest position 134;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-13. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ef650 into BSS @0x3351700. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC5f6f0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC5f6f0 = std::array<std::uint32_t, 4>;

void init_vec_c5f6f0(VecC5f6f0& g) noexcept;

}  // namespace sotc
