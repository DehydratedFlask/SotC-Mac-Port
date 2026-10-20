// Native recovery of original ctor RVA 0xc891e0 (index 178; vector-copy manifest position 117;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-09. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f2cd0 into BSS @0x3351a20. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC891e0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC891e0 = std::array<std::uint32_t, 4>;

void init_vec_c891e0(VecC891e0& g) noexcept;

}  // namespace sotc
