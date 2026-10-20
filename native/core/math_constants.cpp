#include "math_constants.hpp"

#include <bit>

namespace sotc {
namespace {
template <typename T>
void once(T& value, uint64_t& guard, const T& init) noexcept {
    if ((guard & 0xffu) == 0) {   // original: cmp byte [guard],0 ; jne skip  (low byte only), then mov qword [guard],1
        guard = 1;
        value = init;
    }
}
}  // namespace

void init_math_constants(MathConstants& g) noexcept {
    // Bit patterns are the ones stored by the original: 0xbf800000, 0x3f000000, 0x40000000,
    // 0x40490fdb, 0x34000000, 0x7f7fffff.
    once(g.neg_one, g.neg_one_guard, std::bit_cast<float>(0xbf800000u));
    once(g.half, g.half_guard, std::bit_cast<float>(0x3f000000u));
    once(g.two, g.two_guard, std::bit_cast<float>(0x40000000u));
    once(g.pi, g.pi_guard, std::bit_cast<float>(0x40490fdbu));
    once(g.epsilon, g.epsilon_guard, std::bit_cast<float>(0x34000000u));
    once(g.max_float, g.max_float_guard, std::bit_cast<float>(0x7f7fffffu));
    once(g.lane0_mask, g.lane0_mask_guard, std::array<uint32_t, 4>{0xffffffffu, 0, 0, 0});
    once(g.xyz_mask, g.xyz_mask_guard, std::array<uint32_t, 4>{0xffffffffu, 0xffffffffu, 0xffffffffu, 0});
    g.xyz_mask_copy = {0xffffffffu, 0xffffffffu, 0xffffffffu, 0};
}

}  // namespace sotc
