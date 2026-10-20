// Native recovery of original ctor RVA 0xc23650 (index 219; vector-copy manifest position 154;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-17. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12eaae0 into BSS @0x3351580. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC23650Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC23650 = std::array<std::uint32_t, 4>;

void init_vec_c23650(VecC23650& g) noexcept;

}  // namespace sotc
