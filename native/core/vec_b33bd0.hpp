// Native recovery of original ctor RVA 0xb33bd0 (ctor survey index 295; vector-copy manifest
// position 226; ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-25..29. Unguarded 16-byte
// copy of {~0,~0,~0,0} (4x u32 lanes) from rodata @0x12d2f50 into BSS @0x3350c70. Same
// bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecB33BD0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecB33BD0 = std::array<std::uint32_t, 4>;

void init_vec_b33bd0(VecB33BD0& g) noexcept;

}  // namespace sotc
