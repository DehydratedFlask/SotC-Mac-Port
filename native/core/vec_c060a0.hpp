// Native recovery of original ctor RVA 0xc060a0 (index 236; vector-copy manifest position 169;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-20. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e6d30 into BSS @0x33512b0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC060A0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC060A0 = std::array<std::uint32_t, 4>;

void init_vec_c060a0(VecC060A0& g) noexcept;

}  // namespace sotc
