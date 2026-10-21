// Native recovery of original ctor RVA 0xb4ef70 (ctor survey index 291; vector-copy manifest
// position 222; ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-25..29. Unguarded 16-byte
// copy of {~0,~0,~0,0} (4x u32 lanes) from rodata @0x12d65d0 into BSS @0x3350cd0. Same
// bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecB4EF70Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecB4EF70 = std::array<std::uint32_t, 4>;

void init_vec_b4ef70(VecB4EF70& g) noexcept;

}  // namespace sotc
