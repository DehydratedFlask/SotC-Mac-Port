// Native recovery of original ctor RVA 0xc17a70 (index 224; vector-copy manifest position 159;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-18. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e9230 into BSS @0x3351530. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC17A70Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC17A70 = std::array<std::uint32_t, 4>;

void init_vec_c17a70(VecC17A70& g) noexcept;

}  // namespace sotc
