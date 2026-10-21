#include "avx_d5a280.hpp"

namespace sotc {
namespace {
// rodata the original loads (source address in the comment).
constexpr std::uint32_t kBase[8] = {0x0u, 0x1u, 0x2u, 0x3u, 0x4u, 0x5u, 0x6u, 0x7u};   // 0x13030e0: low/high 128-bit lane start (0..7)
constexpr std::uint32_t kPlus8[4] = {0x8u, 0x8u, 0x8u, 0x8u};   // 0x1302ff0: splat 8
constexpr std::uint32_t kPlus16[4] = {0x10u, 0x10u, 0x10u, 0x10u};   // 0x1303000: splat 16
constexpr std::uint32_t kPlus24[4] = {0x18u, 0x18u, 0x18u, 0x18u};   // 0x1303010: splat 24
constexpr std::uint32_t kStep32[4] = {0x20u, 0x20u, 0x20u, 0x20u};   // 0x1303020: splat 32
constexpr std::uint8_t kPrefix[16] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00};   // 0x1302fe0: {~0,~0,~0,0}

void put_u32(std::uint8_t* dst, std::uint32_t v) noexcept {
    for (int i = 0; i != 4; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_avx_d5a280(AvxD5a280& g) noexcept {
    for (int i = 0; i != 16; ++i) g.prefix[i] = kPrefix[i];

    // Decoded loop: 16 iterations, each storing four 32-byte blocks at table + 0x80*i + {0,0x20,0x40,0x60}.
    // Per 128-bit lane the k-th block is base + i*kStep32 + (k == 0 ? 0 : kPlus{8,16,24}).
    std::uint32_t lanes[8];
    for (int lane = 0; lane != 8; ++lane) lanes[lane] = kBase[lane];
    for (int i = 0; i != 16; ++i) {
        const std::uint32_t* adds[4] = {nullptr, kPlus8, kPlus16, kPlus24};
        for (int k = 0; k != 4; ++k) {
            for (int lane = 0; lane != 8; ++lane) {
                const std::uint32_t add = adds[k] ? adds[k][lane & 3] : 0u;
                put_u32(g.table.data() + 0x80 * i + 0x20 * k + 4 * lane, lanes[lane] + add);
            }
        }
        for (int lane = 0; lane != 8; ++lane) lanes[lane] += kStep32[lane & 3];
    }
}

}  // namespace sotc
