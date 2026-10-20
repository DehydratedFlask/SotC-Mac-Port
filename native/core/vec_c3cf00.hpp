// Native recovery of original ctor RVA 0xc3cf00 (index 216; vector-copy manifest position 151;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-16. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ec5b0 into BSS @0x33515f0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC3CF00Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC3CF00 = std::array<std::uint32_t, 4>;

void init_vec_c3cf00(VecC3CF00& g) noexcept;

}  // namespace sotc
