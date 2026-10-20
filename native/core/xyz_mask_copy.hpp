// Native recovery of original ctor RVA 0xf30460 (index 9; ELF sha256 d0723afb...5861d3), task CPU-01.
// Unguarded 16-byte copy of {~0,~0,~0,0} from rodata. Distinct BSS cell from math/trig xyz masks.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kXyzMaskBits{0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x33567d0.
using XyzMaskCopy = std::array<std::uint32_t, 4>;

// Behaviour of 0xf30460: always overwrite the four lanes from the fixed pattern (no guard).
void init_xyz_mask_copy(XyzMaskCopy& g) noexcept;

}  // namespace sotc
