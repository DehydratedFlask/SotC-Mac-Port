// Native recovery of original ctor RVA 0xd71f60 (index 95; ELF sha256 d0723afb...5861d3),
// task CPU-01-CTOR-COVERAGE-BATCH-01. Unguarded 17-byte copy of {~0,~0,~0,0} from rodata
// @0x1305650 into BSS @0x33529c0. Same bit pattern as sibling mask copies, but distinct cells —
// do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD71f60Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

// Stand-in for the original destination cell @0x33529c0.
using VecD71f60 = std::array<std::uint32_t, 4>;

// Behaviour of 0xd71f60: always overwrite the four lanes from the fixed pattern (no guard).
void init_vec_d71f60(VecD71f60& g) noexcept;

}  // namespace sotc
