// Native recovery of original ctor RVA 0xdb93a0 (index 72; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-73721c8d). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x130d940 into
// BSS @0x3352c80. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecDb93a0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecDb93a0 = std::array<std::uint32_t, 4>;

void init_vec_db93a0(VecDb93a0& g) noexcept;

}  // namespace sotc
