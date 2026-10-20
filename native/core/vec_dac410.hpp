// Native recovery of original ctor RVA 0xdac410 (index 77; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-02397584). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x130ca00 into
// BSS @0x3352c30. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecDac410Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352c30.
using VecDac410 = std::array<std::uint32_t, 4>;

// Behaviour of 0xdac410: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_dac410(VecDac410& g) noexcept;

}  // namespace sotc
