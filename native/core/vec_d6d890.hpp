// Native recovery of original ctor RVA 0xd6d890 (index 98; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-81763e53). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x13050b0 into
// BSS @0x3352990. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD6d890Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352990.
using VecD6d890 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd6d890: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d6d890(VecD6d890& g) noexcept;

}  // namespace sotc
