#include "leaf_c6e9d0.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
void put_u32(std::uint8_t* dst, std::uint32_t v) noexcept {
    for (int i = 0; i != 4; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_leaf_c6e9d0(LeafC6e9d0& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x3351840: 32-byte store: lanes 3f800000 00000000 00000000 00000000 3f800000 00000000 00000000 00000000
    put_u64(g.cells.data() + 0x010, 0x000000003f800000ull);
    put_u64(g.cells.data() + 0x018, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x020, 0x000000003f800000ull);
    put_u64(g.cells.data() + 0x028, 0x0000000000000000ull);
    // 0x3351860: 32-byte store: lanes 3f800000 bf800000 00000000 00000000 00000000 bf800000 00000000 00000000
    put_u64(g.cells.data() + 0x030, 0xbf8000003f800000ull);
    put_u64(g.cells.data() + 0x038, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x040, 0xbf80000000000000ull);
    put_u64(g.cells.data() + 0x048, 0x0000000000000000ull);
    // 0x3351880: 4-byte store: 1 x 0x00000000
    put_u32(g.cells.data() + 0x050, 0x00000000u);
    // 0x3351830: 16-byte store: lanes ffffffff ffffffff ffffffff 00000000
    put_u64(g.cells.data() + 0x000, 0xffffffffffffffffull);
    put_u64(g.cells.data() + 0x008, 0x00000000ffffffffull);
    // 0x3351884: 4-byte store: 1 x 0xbf800000 (-1.0f splat)
    put_u32(g.cells.data() + 0x054, 0xbf800000u);
    // 0x33518a0: 32-byte store: lanes 00000000 bf3504f3 bf3504f3 3f3504f3 00000000 bf3504f3 00000000 3f3504f3
    put_u64(g.cells.data() + 0x070, 0xbf3504f300000000ull);
    put_u64(g.cells.data() + 0x078, 0x3f3504f3bf3504f3ull);
    put_u64(g.cells.data() + 0x080, 0xbf3504f300000000ull);
    put_u64(g.cells.data() + 0x088, 0x3f3504f300000000ull);
    // 0x33518c0: 32-byte store: lanes bf3504f3 bf3504f3 00000000 bf3504f3 00000000 3f3504f3 3f3504f3 3f3504f3
    put_u64(g.cells.data() + 0x090, 0xbf3504f3bf3504f3ull);
    put_u64(g.cells.data() + 0x098, 0xbf3504f300000000ull);
    put_u64(g.cells.data() + 0x0a0, 0x3f3504f300000000ull);
    put_u64(g.cells.data() + 0x0a8, 0x3f3504f33f3504f3ull);
    // 0x33518e0: 32-byte store: lanes 00000000 3f3504f3 00000000 bf3504f3 3f3504f3 bf3504f3 00000000 3f3504f3
    put_u64(g.cells.data() + 0x0b0, 0x3f3504f300000000ull);
    put_u64(g.cells.data() + 0x0b8, 0xbf3504f300000000ull);
    put_u64(g.cells.data() + 0x0c0, 0xbf3504f33f3504f3ull);
    put_u64(g.cells.data() + 0x0c8, 0x3f3504f300000000ull);
    // 0x3351900: 32-byte store: lanes 3f3504f3 bf3504f3 00000000 3f3504f3 3f3504f3 00000000 bf3504f3 3f3504f3
    put_u64(g.cells.data() + 0x0d0, 0xbf3504f33f3504f3ull);
    put_u64(g.cells.data() + 0x0d8, 0x3f3504f300000000ull);
    put_u64(g.cells.data() + 0x0e0, 0x000000003f3504f3ull);
    put_u64(g.cells.data() + 0x0e8, 0x3f3504f3bf3504f3ull);
    // 0x3351920: 16-byte store: lanes 00000000 bf3504f3 bf3504f3 00000000
    put_u64(g.cells.data() + 0x0f0, 0xbf3504f300000000ull);
    put_u64(g.cells.data() + 0x0f8, 0x00000000bf3504f3ull);
}

}  // namespace sotc
