// Native recovery of original ctor RVA 0xc741c0 (index 184; vector-copy manifest position 122;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-10. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f1970 into BSS @0x33519b0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC741c0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC741c0 = std::array<std::uint32_t, 4>;

void init_vec_c741c0(VecC741c0& g) noexcept;

}  // namespace sotc
