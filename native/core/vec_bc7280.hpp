// Native recovery of original ctor RVA 0xbc7280 (index 255; vector-copy manifest position 187;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-23. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12df790 into BSS @0x3350ff0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBC7280Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBC7280 = std::array<std::uint32_t, 4>;

void init_vec_bc7280(VecBC7280& g) noexcept;

}  // namespace sotc
