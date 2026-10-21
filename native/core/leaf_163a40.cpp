#include "leaf_163a40.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
void put_u32(std::uint8_t* dst, std::uint32_t v) noexcept {
    for (int i = 0; i != 4; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_leaf_163a40(Leaf163a40& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x2ee3c60: 16-byte store: 4 x 0x00000000
    put_u64(g.cells.data() + 0x000, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x008, 0x0000000000000000ull);
    // 0x2ee3c70: 8-byte store: 2 x 0x00000000
    put_u64(g.cells.data() + 0x010, 0x0000000000000000ull);
    // 0x2ee3c80: 16-byte store: lanes bf000000 bf000000 bf000000 3f000000
    put_u64(g.cells.data() + 0x020, 0xbf000000bf000000ull);
    put_u64(g.cells.data() + 0x028, 0x3f000000bf000000ull);
    // 0x2ee3c90: 4-byte store: 1 x 0x3f000000 (0.5f splat)
    put_u32(g.cells.data() + 0x030, 0x3f000000u);
    // 0x2ee3ca0: 16-byte store: lanes 7f7fffff 7f7fffff 7f7fffff ff7fffff
    put_u64(g.cells.data() + 0x040, 0x7f7fffff7f7fffffull);
    put_u64(g.cells.data() + 0x048, 0xff7fffff7f7fffffull);
    // 0x2ee3cb0: 4-byte store: 1 x 0xff7fffff
    put_u32(g.cells.data() + 0x050, 0xff7fffffu);
    // 0x2ee3c94: 4-byte store: 1 x 0x3f000000 (0.5f splat)
    put_u32(g.cells.data() + 0x034, 0x3f000000u);
    // 0x2ee3cb4: 4-byte store: 1 x 0xff7fffff
    put_u32(g.cells.data() + 0x054, 0xff7fffffu);
    // 0x2ee3cc0: 16-byte store: lanes ff7fffff ff7fffff ff7fffff 7f7fffff
    put_u64(g.cells.data() + 0x060, 0xff7fffffff7fffffull);
    put_u64(g.cells.data() + 0x068, 0x7f7fffffff7fffffull);
    // 0x2ee3cd0: 8-byte store: 2 x 0x7f7fffff
    put_u64(g.cells.data() + 0x070, 0x7f7fffff7f7fffffull);
}

}  // namespace sotc
