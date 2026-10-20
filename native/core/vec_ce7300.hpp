// Native recovery of original ctor RVA 0xce7300 (ctor survey position 149; vector-copy manifest
// position 91; ELF SHA256 d0723afb...5861d3), task CPU-01d (bc-effcf5dc). Unguarded 16-byte copy
// of {~0,~0,~0,0} from rodata @0x12f8bb0 into
// BSS @0x3351cf0. Same bit pattern as sibling mask copies, but distinct cells — do not merge.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCe7300Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0};

using VecCe7300 = std::array<std::uint32_t, 4>;

void init_vec_ce7300(VecCe7300& g) noexcept;

}  // namespace sotc
