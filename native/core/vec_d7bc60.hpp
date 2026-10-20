// Native recovery of original ctor RVA 0xd7bc60 (survey index 93; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-effcf5dc). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x1306860 into
// BSS @0x33529e0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD7bc60Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x33529e0.
using VecD7bc60 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd7bc60: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d7bc60(VecD7bc60& g) noexcept;

}  // namespace sotc
