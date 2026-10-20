// Native recovery of original ctor RVA 0xd32b70 (index 118; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-73721c8d). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x1300d30 into
// BSS @0x3351ee0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD32b70Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecD32b70 = std::array<std::uint32_t, 4>;

void init_vec_d32b70(VecD32b70& g) noexcept;

}  // namespace sotc
