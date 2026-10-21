#include "guarded_dd4210.hpp"

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

void init_guarded_dd4210(GuardedDd4210& g) noexcept {
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
    // Unguarded tail (always rewritten). Original cells 0x3352d30..0x3352db0.
    // 0x3352d90: one 4-byte store
    put_u32(g.tail.data() + 0x60, 0x40fff7cfu);
    // 0x3352d94: one 4-byte store
    put_u32(g.tail.data() + 0x64, 0x38d1b717u);
    // 0x3352d60: one 8-byte store
    put_u64(g.tail.data() + 0x30, 0x0000000000000000ull);
    // 0x3352d70: one 8-byte store
    put_u64(g.tail.data() + 0x40, 0x0000000000000000ull);
    // 0x3352d30: one 16-byte store (split into 8-byte native writes)
    put_u64(g.tail.data() + 0x00, 0x8000000080000000ull);
    put_u64(g.tail.data() + 0x08, 0x8000000080000000ull);
    // 0x3352d78: one 8-byte store
    put_u64(g.tail.data() + 0x48, 0xffffffff00000000ull);
    // 0x3352d40: one 16-byte store (split into 8-byte native writes)
    put_u64(g.tail.data() + 0x10, 0x8000000780000007ull);
    put_u64(g.tail.data() + 0x18, 0x8000000780000007ull);
    // 0x3352da0: one 8-byte store
    put_u64(g.tail.data() + 0x70, 0xff7fffffff7fffffull);
    // 0x3352d50: one 8-byte store
    put_u64(g.tail.data() + 0x20, 0x00000000ffffffffull);
    // 0x3352d68: one 8-byte store
    put_u64(g.tail.data() + 0x38, 0x00000000ffffffffull);
    // 0x3352d80: one 8-byte store
    put_u64(g.tail.data() + 0x50, 0x3f8000003f800000ull);
    // 0x3352d58: one 8-byte store
    put_u64(g.tail.data() + 0x28, 0x0000000000000000ull);
    // 0x3352d88: one 8-byte store
    put_u64(g.tail.data() + 0x58, 0x000000003f800000ull);
    // 0x3352da8: one 8-byte store
    put_u64(g.tail.data() + 0x78, 0x00000000ff7fffffull);
}

}  // namespace sotc
