// Native recovery of original ctor RVA 0xd85490 (index 82; vector-copy manifest position 28;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-21. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x130a970 into BSS @0x3352a90. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecD85490Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecD85490 = std::array<std::uint32_t, 4>;

void init_vec_d85490(VecD85490& g) noexcept;

}  // namespace sotc
