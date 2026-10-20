// Native recovery of original ctor RVA 0xd7fe40 (survey index 87; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-effcf5dc). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x13077e0 into
// BSS @0x3352a40. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD7fe40Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352a40.
using VecD7fe40 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd7fe40: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d7fe40(VecD7fe40& g) noexcept;

}  // namespace sotc
