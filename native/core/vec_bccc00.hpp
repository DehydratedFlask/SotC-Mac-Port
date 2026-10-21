// Native recovery of original ctor RVA 0xbccc00 (index 250; vector-copy manifest position 182;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-22. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e0680 into BSS @0x3351140. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBCCC00Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBCCC00 = std::array<std::uint32_t, 4>;

void init_vec_bccc00(VecBCCC00& g) noexcept;

}  // namespace sotc
