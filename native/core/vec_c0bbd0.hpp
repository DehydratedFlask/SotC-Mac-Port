// Native recovery of original ctor RVA 0xc0bbd0 (index 231; vector-copy manifest position 164;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-19. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e79c0 into BSS @0x3351300. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC0BBD0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC0BBD0 = std::array<std::uint32_t, 4>;

void init_vec_c0bbd0(VecC0BBD0& g) noexcept;

}  // namespace sotc
