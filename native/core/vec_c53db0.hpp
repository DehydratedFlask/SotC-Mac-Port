// Native recovery of original ctor RVA 0xc53db0 (index 208; vector-copy manifest position 143;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-15. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ee370 into BSS @0x3351670. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC53DB0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC53DB0 = std::array<std::uint32_t, 4>;

void init_vec_c53db0(VecC53DB0& g) noexcept;

}  // namespace sotc
