// Native recovery of original ctor RVA 0xd62b00 (index 101; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-effcf5dc). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x1304610 into
// BSS @0x3352960. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD62b00Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3352960.
using VecD62b00 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd62b00: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d62b00(VecD62b00& g) noexcept;

}  // namespace sotc
