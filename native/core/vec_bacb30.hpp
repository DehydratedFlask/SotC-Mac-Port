// Native recovery of original ctor RVA 0xbacb30 (ctor survey index 269; vector-copy manifest
// position 201; ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-25..29. Unguarded 16-byte
// copy of {~0,~0,~0,0} (4x u32 lanes) from rodata @0x12dcff0 into BSS @0x3350e80. Same
// bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBACB30Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBACB30 = std::array<std::uint32_t, 4>;

void init_vec_bacb30(VecBACB30& g) noexcept;

}  // namespace sotc
