// Native recovery of original ctor RVA 0xbbd5e0 (index 261; vector-copy manifest position 193;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-24. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12de320 into BSS @0x3350f40. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBBD5E0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBBD5E0 = std::array<std::uint32_t, 4>;

void init_vec_bbd5e0(VecBBD5E0& g) noexcept;

}  // namespace sotc
