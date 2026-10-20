// Native recovery of original ctor RVA 0xc64060 (index 197; vector-copy manifest position 132;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-13. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12efbf0 into BSS @0x3351720. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC64060Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC64060 = std::array<std::uint32_t, 4>;

void init_vec_c64060(VecC64060& g) noexcept;

}  // namespace sotc
