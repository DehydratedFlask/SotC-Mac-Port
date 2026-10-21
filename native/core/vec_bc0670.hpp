// Native recovery of original ctor RVA 0xbc0670 (index 259; vector-copy manifest position 191;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-24. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12de6c0 into BSS @0x3350f60. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBC0670Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBC0670 = std::array<std::uint32_t, 4>;

void init_vec_bc0670(VecBC0670& g) noexcept;

}  // namespace sotc
