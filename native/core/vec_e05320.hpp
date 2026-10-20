// Native recovery of original ctor RVA 0xe05320 (index 52; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-dc141fdf). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x13132d0 into
// BSS @0x3353360. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecE05320Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3353360.
using VecE05320 = std::array<std::uint32_t, 4>;

// Behaviour of 0xe05320: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_e05320(VecE05320& g) noexcept;

}  // namespace sotc
