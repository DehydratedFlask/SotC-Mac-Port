// Native recovery of original ctor RVA 0xd6dbf0 (index 97; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-73721c8d). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x13052d0 into
// BSS @0x33529a0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD6dbf0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecD6dbf0 = std::array<std::uint32_t, 4>;

void init_vec_d6dbf0(VecD6dbf0& g) noexcept;

}  // namespace sotc
