// Native recovery of original ctor RVA 0xc54d20 (index 206; vector-copy manifest position 141;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-14. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ee790 into BSS @0x3351690. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC54D20Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC54D20 = std::array<std::uint32_t, 4>;

void init_vec_c54d20(VecC54D20& g) noexcept;

}  // namespace sotc
