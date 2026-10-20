// Native recovery of original ctor RVA 0xde7fb0 (survey index 56; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-effcf5dc). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x1310c90 into
// BSS @0x3353320. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecDe7fb0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3353320.
using VecDe7fb0 = std::array<std::uint32_t, 4>;

// Behaviour of 0xde7fb0: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_de7fb0(VecDe7fb0& g) noexcept;

}  // namespace sotc
