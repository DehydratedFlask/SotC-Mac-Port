// Native recovery of original ctor RVA 0xc54830 (index 207; vector-copy manifest position 142;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-15. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12ee5f0 into BSS @0x3351680. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC54830Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC54830 = std::array<std::uint32_t, 4>;

void init_vec_c54830(VecC54830& g) noexcept;

}  // namespace sotc
