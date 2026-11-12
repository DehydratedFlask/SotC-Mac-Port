// CPU-01-STARTUP-SLOT-505: see bounds_minmax_1621b0.hpp.
#include "bounds_minmax_1621b0.hpp"

#include <cstring>

namespace sotc {
namespace {

bool is_nan_bits(float value) noexcept {
    std::uint32_t bits;
    std::memcpy(&bits, &value, sizeof bits);
    return (bits & 0x7f800000u) == 0x7f800000u && (bits & 0x007fffffu) != 0;
}

}  // namespace

float bounds_minss(float current, float incoming) noexcept {
    // VMINSS: if either operand is NaN, return the second/source operand. Otherwise return the
    // smaller operand; equality returns the second operand (including signed-zero cases).
    if (is_nan_bits(current) || is_nan_bits(incoming)) return incoming;
    return current < incoming ? current : incoming;
}

float bounds_maxss(float current, float incoming) noexcept {
    // VMAXSS has the same NaN/second-operand rule and the same equality rule.
    if (is_nan_bits(current) || is_nan_bits(incoming)) return incoming;
    return current > incoming ? current : incoming;
}

void bounds_minmax_1621b0(BoundsRecord& bounds, const BoundsVec3& point) noexcept {
    bounds.min.x = bounds_minss(bounds.min.x, point.x);
    bounds.min.y = bounds_minss(bounds.min.y, point.y);
    bounds.min.z = bounds_minss(bounds.min.z, point.z);
    bounds.max.x = bounds_maxss(bounds.max.x, point.x);
    bounds.max.y = bounds_maxss(bounds.max.y, point.y);
    bounds.max.z = bounds_maxss(bounds.max.z, point.z);
}

const BoundsRecord& bounds_seed_162970() noexcept {
    // 0x2ee3ca0 is zero-filled BSS at the time slot 505 runs; slot 526 writes it later.
    static const BoundsRecord zero_seed{};
    return zero_seed;
}

}  // namespace sotc
