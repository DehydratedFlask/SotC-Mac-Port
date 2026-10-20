// Native recovery of original ctor RVA 0xcdc760 (ctor survey position 157; vector-copy manifest
// position 99; ELF SHA256 d0723afb...5861d3), task CPU-01d. Unguarded 16-byte copy of
// {~0,~0,~0,0} from rodata @0x12f7440 into BSS @0x3351c70. These are distinct cells; do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCdc760Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCdc760 = std::array<std::uint32_t, 4>;

void init_vec_cdc760(VecCdc760& g) noexcept;

}  // namespace sotc
