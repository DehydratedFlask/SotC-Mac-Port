// Native recovery of original ctor RVA 0xc20710 (index 221; vector-copy manifest position 156;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-17. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12e9d10 into BSS @0x3351560. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC20710Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC20710 = std::array<std::uint32_t, 4>;

void init_vec_c20710(VecC20710& g) noexcept;

}  // namespace sotc
