#include "guarded_d33b50.hpp"

#include <bit>

namespace sotc {
namespace {
template <typename T>
void once(T& value, std::uint64_t& guard, const T& init) noexcept {
    if ((guard & 0xffu) == 0) {   // original: cmp byte [guard],0 ; jne skip ; mov qword [guard],1
        guard = 1;
        value = init;
    }
}
}  // namespace

void init_guarded_d33b50(GuardedD33b50& g) noexcept {
    once(g.neg_one, g.neg_one_guard, std::bit_cast<float>(0xbf800000u));
    once(g.half, g.half_guard, std::bit_cast<float>(0x3f000000u));
    once(g.two, g.two_guard, std::bit_cast<float>(0x40000000u));
    once(g.pi, g.pi_guard, std::bit_cast<float>(0x40490fdbu));
    once(g.epsilon, g.epsilon_guard, std::bit_cast<float>(0x34000000u));
    once(g.max_float, g.max_float_guard, std::bit_cast<float>(0x7f7fffffu));
    once(g.lane0_mask, g.lane0_mask_guard,
         std::array<std::uint32_t, 4>{0xffffffffu, 0, 0, 0});
    once(g.xyz_mask, g.xyz_mask_guard,
         std::array<std::uint32_t, 4>{0xffffffffu, 0xffffffffu, 0xffffffffu, 0});
    once(g.one, g.one_guard, std::uint32_t{1u});
}

}  // namespace sotc
