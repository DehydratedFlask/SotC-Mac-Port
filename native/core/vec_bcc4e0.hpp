// Native recovery of original ctor RVA 0xbcc4e0 (index 252; vector-copy manifest position 184;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-23. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e0350 into BSS @0x3351120. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBCC4E0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBCC4E0 = std::array<std::uint32_t, 4>;

void init_vec_bcc4e0(VecBCC4E0& g) noexcept;

}  // namespace sotc
