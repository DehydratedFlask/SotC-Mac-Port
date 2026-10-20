// Native recovery of original ctor RVA 0xcc5a10 (index 164; vector-copy manifest position 106;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-05. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f5c80 into BSS @0x3351c00. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCc5a10Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCc5a10 = std::array<std::uint32_t, 4>;

void init_vec_cc5a10(VecCc5a10& g) noexcept;

}  // namespace sotc
