// Native recovery of original function RVA 0xe10490 (ELF sha256 d0723afb...5861d3), task CPU-01.
// Third recovered constructor; first to call a libm routine. Names are ours.
#pragma once
#include <array>
#include <cstdint>

namespace sotc {

struct TrigConstants {
    std::array<std::uint32_t, 4> xyz_mask;   // {~0, ~0, ~0, 0}, unguarded 16-byte copy
    float cos_3_degrees;                     // _FSin(3 deg in radians as float, offset 1, 0)  [threshold-sensitive, low: only reader compares dot > this; see HANDOFF cp7]
};

// 3 degrees expressed as the float the original loads from rodata (bit pattern 0x3d567750).
inline constexpr std::uint32_t kThreeDegreesBits = 0x3d567750u;

void init_trig_constants(TrigConstants& g) noexcept;

}  // namespace sotc
