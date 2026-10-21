// Native recovery of original ctor RVA 0xb71f80 (ctor survey index 283; vector-copy manifest
// position 215; ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-25..29. Unguarded 16-byte
// copy of {~0,~0,~0,0} (4x u32 lanes) from rodata @0x12d8b10 into BSS @0x3350da0. Same
// bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecB71F80Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecB71F80 = std::array<std::uint32_t, 4>;

void init_vec_b71f80(VecB71F80& g) noexcept;

}  // namespace sotc
