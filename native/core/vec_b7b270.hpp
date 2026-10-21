// Native recovery of original ctor RVA 0xb7b270 (ctor survey index 276; vector-copy manifest
// position 208; ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-25..29. Unguarded 16-byte
// copy of {~0,~0,~0,0} (4x u32 lanes) from rodata @0x12d9b30 into BSS @0x3350e10. Same
// bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecB7B270Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecB7B270 = std::array<std::uint32_t, 4>;

void init_vec_b7b270(VecB7B270& g) noexcept;

}  // namespace sotc
