// Native recovery of original ctor RVA 0xce57b0 (index 153; ELF SHA256
// d0723afb...c25861d3). It unconditionally copies four u32 lanes into its own
// destination cell at 0x3351cb0.
#pragma once

#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::array<std::uint32_t, 4> kVecCe57B0Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0u,
};

using VecCe57B0 = std::array<std::uint32_t, 4>;

void init_vec_ce57b0(VecCe57B0& cell) noexcept;

}  // namespace sotc
