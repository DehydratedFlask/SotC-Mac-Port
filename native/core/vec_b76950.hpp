// Native recovery of original ctor RVA 0xb76950 (index 280; ELF sha256 d0723afb...5861d3),
// task CPU-01d (claim-b76950). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x12d9350
// into BSS @0x3350dd0. Same bit pattern as sibling mask copies; distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecB76950Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x3350dd0.
using VecB76950 = std::array<std::uint32_t, 4>;

// Behaviour of 0xb76950: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_b76950(VecB76950& g) noexcept;

}  // namespace sotc
