// Native recovery of original ctor RVA 0xdd41e0 (ctor index 67; ELF sha256 d0723afb...5861d3), task CPU-01d.
// Guarded 16-byte splat of 0xbeb851ea (~ -0.36f). Same bit pattern as 0xf4b510 / VecNeg036, but distinct
// BSS cells (value @0x3352cd0, guard @0x3352ce0; source rodata @0x130eac0). Names are ours.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::uint32_t kNeg036BitsDd41 = 0xbeb851eau;  // OBS rodata @0x130eac0

struct VecDd41e0 {
    std::array<std::uint32_t, 4> lanes{};  // original value cell @0x3352cd0
    std::uint64_t guard = 0;               // original guard @0x3352ce0 (byte tested, qword stored)
};

// Behaviour of 0xdd41e0: if guard byte == 0, copy the four identical u32 lanes and set guard = 1.
void init_vec_dd41e0(VecDd41e0& g) noexcept;

}  // namespace sotc
