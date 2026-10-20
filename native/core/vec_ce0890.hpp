// Native recovery of original ctor RVA 0xce0890 (survey index 155).
// The original body unconditionally copies one 16-byte {~0, ~0, ~0, 0} value.
#pragma once

#include <array>
#include <cstdint>

namespace sotc {

using VecCe0890 = std::array<std::uint32_t, 4>;

inline constexpr VecCe0890 kVecCe0890Bits{
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0u};

// Stand-in for the constructor's distinct destination cell at 0x3351c90.
void init_vec_ce0890(VecCe0890& destination) noexcept;

}  // namespace sotc
