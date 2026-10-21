#include "guarded_dd7e30.hpp"

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

void put_u32(std::uint8_t* dst, std::uint32_t v) noexcept {
    for (int i = 0; i != 4; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}

void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}

}  // namespace

void init_guarded_dd7e30(GuardedDd7e30& g) noexcept {
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
    // Unguarded tail (always rewritten). Original cells 0x3352e00..0x3352e40.
    // 0x3352e20: one 4-byte store
    put_u32(g.tail.data() + 0x20, 0x34000000u);
    // 0x3352e00: one 8-byte store
    put_u64(g.tail.data() + 0x00, 0xffffffffffffffffull);
    // 0x3352e30: one 8-byte store
    put_u64(g.tail.data() + 0x30, 0xff7fffffff7fffffull);
    // 0x3352e08: one 8-byte store
    put_u64(g.tail.data() + 0x08, 0x00000000ffffffffull);
    // 0x3352e10: one 8-byte store
    put_u64(g.tail.data() + 0x10, 0x0000000000000000ull);
    // 0x3352e18: one 8-byte store
    put_u64(g.tail.data() + 0x18, 0xffffffff00000000ull);
    // 0x3352e38: one 8-byte store
    put_u64(g.tail.data() + 0x38, 0x00000000ff7fffffull);
}

}  // namespace sotc
