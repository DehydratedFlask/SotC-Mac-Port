// Native recovery of original ctor RVA 0xb6e6d0 (ctor survey index 284; vector-copy manifest
// position 216; ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-25..29. Unguarded 16-byte
// copy of {~0,~0,~0,0} (4x u32 lanes) from rodata @0x12d88b0 into BSS @0x3350d90. Same
// bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecB6E6D0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecB6E6D0 = std::array<std::uint32_t, 4>;

void init_vec_b6e6d0(VecB6E6D0& g) noexcept;

}  // namespace sotc
