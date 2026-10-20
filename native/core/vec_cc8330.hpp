// Native recovery of original ctor RVA 0xcc8330 (index 163; vector-copy manifest position 105;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-05. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f5fd0 into BSS @0x3351c10. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCc8330Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCc8330 = std::array<std::uint32_t, 4>;

void init_vec_cc8330(VecCc8330& g) noexcept;

}  // namespace sotc
