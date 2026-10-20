// Native recovery of original ctor RVA 0xc0aae0 (index 234; vector-copy manifest position 167;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-19. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e7210 into BSS @0x33512d0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC0AAE0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC0AAE0 = std::array<std::uint32_t, 4>;

void init_vec_c0aae0(VecC0AAE0& g) noexcept;

}  // namespace sotc
