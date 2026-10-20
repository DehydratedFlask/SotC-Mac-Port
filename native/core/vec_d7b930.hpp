// Native recovery of original ctor RVA 0xd7b930 (index 94; ELF sha256 d0723afb...5861d3),
// task CPU-01d (bc-02397584). Unguarded 16-byte copy of {~0,~0,~0,0} from rodata @0x1306100 into
// BSS @0x33529d0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD7b930Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x33529d0.
using VecD7b930 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd7b930: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d7b930(VecD7b930& g) noexcept;

}  // namespace sotc
