// Native recovery of original ctor RVA 0xe026b0 (index 53; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-02397584). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x1312f10 into
// BSS @0x3353350. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecE026b0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3353350.
using VecE026b0 = std::array<std::uint32_t, 4>;

// Behaviour of 0xe026b0: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_e026b0(VecE026b0& g) noexcept;

}  // namespace sotc
