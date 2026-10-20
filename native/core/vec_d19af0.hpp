// Native recovery of original ctor RVA 0xd19af0 (ctor-survey.json index 67; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-effcf5dc). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12fec00 into
// BSS @0x3351e70. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD19af0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3351e70.
using VecD19af0 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd19af0: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d19af0(VecD19af0& g) noexcept;

}  // namespace sotc
