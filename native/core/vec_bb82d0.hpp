// Native recovery of original ctor RVA 0xbb82d0 (ctor survey index 264; vector-copy manifest
// position 196; ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-25..29. Unguarded 16-byte
// copy of {~0,~0,~0,0} (4x u32 lanes) from rodata @0x12ddce0 into BSS @0x3350f10. Same
// bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBB82D0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBB82D0 = std::array<std::uint32_t, 4>;

void init_vec_bb82d0(VecBB82D0& g) noexcept;

}  // namespace sotc
