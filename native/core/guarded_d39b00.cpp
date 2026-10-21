#include "guarded_d39b00.hpp"

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

void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}

}  // namespace

void init_guarded_d39b00(GuardedD39b00& g) noexcept {
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
    // Unguarded tail (always rewritten). Original cells 0x3352020..0x3352090.
    // 0x3352020: one 8-byte store
    put_u64(g.tail.data() + 0x00, 0x0000000000000000ull);
    // 0x3352030: one 8-byte store
    put_u64(g.tail.data() + 0x10, 0xffffffffffffffffull);
    // 0x3352038: one 8-byte store
    put_u64(g.tail.data() + 0x18, 0x0000000000000000ull);
    // 0x3352070: one 8-byte store
    put_u64(g.tail.data() + 0x50, 0x0000000000000000ull);
    // 0x3352040: one 8-byte store
    put_u64(g.tail.data() + 0x20, 0xffffffffffffffffull);
    // 0x3352060: one 8-byte store
    put_u64(g.tail.data() + 0x40, 0xbf800000bf800000ull);
    // 0x3352068: one 8-byte store
    put_u64(g.tail.data() + 0x48, 0x3f800000bf800000ull);
    // 0x3352050: one 8-byte store
    put_u64(g.tail.data() + 0x30, 0xffffffff00000000ull);
    // 0x3352028: one 8-byte store
    put_u64(g.tail.data() + 0x08, 0xffffffff00000000ull);
    // 0x3352048: one 8-byte store
    put_u64(g.tail.data() + 0x28, 0x00000000ffffffffull);
    // 0x3352058: one 8-byte store
    put_u64(g.tail.data() + 0x38, 0xffffffffffffffffull);
    // 0x3352080: one 8-byte store
    put_u64(g.tail.data() + 0x60, 0xff7fffffff7fffffull);
    // 0x3352078: one 8-byte store
    put_u64(g.tail.data() + 0x58, 0x7f7fffff00000000ull);
    // 0x3352088: one 8-byte store
    put_u64(g.tail.data() + 0x68, 0x00000000ff7fffffull);
}

}  // namespace sotc
