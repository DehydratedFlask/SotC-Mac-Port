// Native recovery of original ctor RVA 0xddab00 (index 62; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-02397584). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x130f090 into
// BSS @0x3352eb0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecDdab00Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352eb0.
using VecDdab00 = std::array<std::uint32_t, 4>;

// Behaviour of 0xddab00: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_ddab00(VecDdab00& g) noexcept;

}  // namespace sotc
