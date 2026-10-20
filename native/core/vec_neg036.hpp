// Native recovery of original ctor RVA 0xf4b510 (index 7; ELF sha256 d0723afb...5861d3), task CPU-01.
// Guarded 16-byte splat of bit pattern 0xbeb851ea (~ -0.36f). Names are ours.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

inline constexpr std::uint32_t kNeg036Bits = 0xbeb851eau;  // OBS rodata @0x132c8f0

struct VecNeg036 {
    std::array<std::uint32_t, 4> lanes{};  // original value cell @0x3356830
    std::uint64_t guard = 0;               // original guard @0x3356840 (byte tested, qword stored)
};

// Behaviour of 0xf4b510: if guard byte == 0, copy the four identical u32 lanes and set guard = 1.
void init_vec_neg036(VecNeg036& g) noexcept;

}  // namespace sotc
