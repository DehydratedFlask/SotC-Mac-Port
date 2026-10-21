// Native recovery of original ctor RVA 0xb44580 (ctor survey index 294; vector-copy manifest
// position 225; ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-25..29. Unguarded 16-byte
// copy of {~0,~0,~0,0} (4x u32 lanes) from rodata @0x12d3880 into BSS @0x3350ca0. Same
// bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecB44580Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecB44580 = std::array<std::uint32_t, 4>;

void init_vec_b44580(VecB44580& g) noexcept;

}  // namespace sotc
