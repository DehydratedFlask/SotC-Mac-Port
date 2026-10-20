// Native recovery of original ctor RVA 0xc6d910 (index 189; vector-copy manifest position 125;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-11. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f0ea0 into BSS @0x3351820. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC6d910Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC6d910 = std::array<std::uint32_t, 4>;

void init_vec_c6d910(VecC6d910& g) noexcept;

}  // namespace sotc
