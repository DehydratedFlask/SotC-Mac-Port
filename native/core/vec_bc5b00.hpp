// Native recovery of original ctor RVA 0xbc5b00 (index 256; vector-copy manifest position 188;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-23. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12df360 into BSS @0x3350fe0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecBC5B00Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecBC5B00 = std::array<std::uint32_t, 4>;

void init_vec_bc5b00(VecBC5B00& g) noexcept;

}  // namespace sotc
