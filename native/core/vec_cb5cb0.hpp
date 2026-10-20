// Native recovery of original ctor RVA 0xcb5cb0 (index 167; vector-copy manifest position 109;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-06. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f4c80 into BSS @0x3351bd0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCb5cb0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCb5cb0 = std::array<std::uint32_t, 4>;

void init_vec_cb5cb0(VecCb5cb0& g) noexcept;

}  // namespace sotc
