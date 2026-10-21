// Native recovery of original ctor RVA 0xb77e90 (ctor survey index 277; vector-copy manifest
// position 209; ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-25..29. Unguarded 16-byte
// copy of {~0,~0,~0,0} (4x u32 lanes) from rodata @0x12d9970 into BSS @0x3350e00. Same
// bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecB77E90Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecB77E90 = std::array<std::uint32_t, 4>;

void init_vec_b77e90(VecB77E90& g) noexcept;

}  // namespace sotc
