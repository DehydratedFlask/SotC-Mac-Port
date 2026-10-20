// Native recovery of original ctor RVA 0xc42b00 (index 214; vector-copy manifest position 149;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-16. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ed210 into BSS @0x3351610. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC42B00Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC42B00 = std::array<std::uint32_t, 4>;

void init_vec_c42b00(VecC42B00& g) noexcept;

}  // namespace sotc
