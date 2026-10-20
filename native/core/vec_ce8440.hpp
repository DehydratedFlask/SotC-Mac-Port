// Native recovery of original ctor RVA 0xce8440 (index 147; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-9aa0fe88). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12f9100 into
// BSS @0x3351d10. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCe8440Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCe8440 = std::array<std::uint32_t, 4>;

void init_vec_ce8440(VecCe8440& g) noexcept;

}  // namespace sotc
