// Native recovery of original ctor RVA 0xddef00 (index 60; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-02397584). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x130f540 into
// BSS @0x3352ed0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecDdef00Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352ed0.
using VecDdef00 = std::array<std::uint32_t, 4>;

// Behaviour of 0xddef00: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_ddef00(VecDdef00& g) noexcept;

}  // namespace sotc
