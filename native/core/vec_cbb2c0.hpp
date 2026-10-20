// Native recovery of original ctor RVA 0xcbb2c0 (index 166; vector-copy manifest position 108;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-06. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f5330 into BSS @0x3351be0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCbb2c0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCbb2c0 = std::array<std::uint32_t, 4>;

void init_vec_cbb2c0(VecCbb2c0& g) noexcept;

}  // namespace sotc
