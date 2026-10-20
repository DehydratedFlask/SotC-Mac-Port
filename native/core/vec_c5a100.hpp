// Native recovery of original ctor RVA 0xc5a100 (index 204; vector-copy manifest position 139;
// ELF sha256 d0723afb...5861d3), task CPU-01-LEAF-CTOR-BATCH-14. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12eec70 into BSS @0x33516b0. Same bit pattern as sibling mask
// copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecC5A100Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecC5A100 = std::array<std::uint32_t, 4>;

void init_vec_c5a100(VecC5A100& g) noexcept;

}  // namespace sotc
