// Native recovery of original ctor RVA 0xe094e0 (index 50; vector-copy manifest position 4;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-21. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x1313e20 into BSS @0x3353380. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecE094E0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecE094E0 = std::array<std::uint32_t, 4>;

void init_vec_e094e0(VecE094E0& g) noexcept;

}  // namespace sotc
