// Native recovery of original ctor RVA 0xbe5410 (index 238; vector-copy manifest position 170;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-20. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e35c0 into BSS @0x3351280. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBE5410Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBE5410 = std::array<std::uint32_t, 4>;

void init_vec_be5410(VecBE5410& g) noexcept;

}  // namespace sotc
