// Native recovery of original ctor RVA 0xc61670 (index 198; vector-copy manifest position 133;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-13. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ef8c0 into BSS @0x3351710. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC61670Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC61670 = std::array<std::uint32_t, 4>;

void init_vec_c61670(VecC61670& g) noexcept;

}  // namespace sotc
