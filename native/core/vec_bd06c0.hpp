// Native recovery of original ctor RVA 0xbd06c0 (index 248; vector-copy manifest position 180;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-22. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e0ee0 into BSS @0x3351160. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBD06C0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBD06C0 = std::array<std::uint32_t, 4>;

void init_vec_bd06c0(VecBD06C0& g) noexcept;

}  // namespace sotc
