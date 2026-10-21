#include "avx_d86ed0.hpp"

namespace sotc {
namespace {
void put_u32(std::uint8_t* dst, std::uint32_t v) noexcept {
    for (int i = 0; i != 4; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}

void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_avx_d86ed0(AvxD86ed0& g) noexcept {
    // Unguarded: the original rewrites these cells on every call.
    // 0x3352ac0: one 32-byte store (split into 8-byte native writes)
    put_u64(g.cells.data() + 0x020, 0x000000003f800000ull);
    put_u64(g.cells.data() + 0x028, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x030, 0x000000003f800000ull);
    put_u64(g.cells.data() + 0x038, 0x0000000000000000ull);
    // 0x3352ae0: one 32-byte store (split into 8-byte native writes)
    put_u64(g.cells.data() + 0x040, 0xbf8000003f800000ull);
    put_u64(g.cells.data() + 0x048, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x050, 0xbf80000000000000ull);
    put_u64(g.cells.data() + 0x058, 0x0000000000000000ull);
    // 0x3352b00: one 4-byte store
    put_u32(g.cells.data() + 0x060, 0x00000000u);
    // 0x3352aa0: one 16-byte store (split into 8-byte native writes)
    put_u64(g.cells.data() + 0x000, 0xffffffffffffffffull);
    put_u64(g.cells.data() + 0x008, 0x00000000ffffffffull);
    // 0x3352b04: one 4-byte store
    put_u32(g.cells.data() + 0x064, 0xbf800000u);
    // 0x3352b20: one 32-byte store (split into 8-byte native writes)
    put_u64(g.cells.data() + 0x080, 0xbf3504f300000000ull);
    put_u64(g.cells.data() + 0x088, 0x3f3504f3bf3504f3ull);
    put_u64(g.cells.data() + 0x090, 0xbf3504f300000000ull);
    put_u64(g.cells.data() + 0x098, 0x3f3504f300000000ull);
    // 0x3352b40: one 32-byte store (split into 8-byte native writes)
    put_u64(g.cells.data() + 0x0a0, 0xbf3504f3bf3504f3ull);
    put_u64(g.cells.data() + 0x0a8, 0xbf3504f300000000ull);
    put_u64(g.cells.data() + 0x0b0, 0x3f3504f300000000ull);
    put_u64(g.cells.data() + 0x0b8, 0x3f3504f33f3504f3ull);
    // 0x3352b60: one 32-byte store (split into 8-byte native writes)
    put_u64(g.cells.data() + 0x0c0, 0x3f3504f300000000ull);
    put_u64(g.cells.data() + 0x0c8, 0xbf3504f300000000ull);
    put_u64(g.cells.data() + 0x0d0, 0xbf3504f33f3504f3ull);
    put_u64(g.cells.data() + 0x0d8, 0x3f3504f300000000ull);
    // 0x3352b80: one 32-byte store (split into 8-byte native writes)
    put_u64(g.cells.data() + 0x0e0, 0xbf3504f33f3504f3ull);
    put_u64(g.cells.data() + 0x0e8, 0x3f3504f300000000ull);
    put_u64(g.cells.data() + 0x0f0, 0x000000003f3504f3ull);
    put_u64(g.cells.data() + 0x0f8, 0x3f3504f3bf3504f3ull);
    // 0x3352ba0: one 16-byte store (split into 8-byte native writes)
    put_u64(g.cells.data() + 0x100, 0xbf3504f300000000ull);
    put_u64(g.cells.data() + 0x108, 0x00000000bf3504f3ull);
}

}  // namespace sotc
