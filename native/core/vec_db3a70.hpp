// Native recovery of original ctor RVA 0xdb3a70 (index 75; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-02397584). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x130d1e0 into
// BSS @0x3352c50. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecDb3a70Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352c50.
using VecDb3a70 = std::array<std::uint32_t, 4>;

// Behaviour of 0xdb3a70: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_db3a70(VecDb3a70& g) noexcept;

}  // namespace sotc
