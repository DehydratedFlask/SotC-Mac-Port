#include "leaf_3ca7c0.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_leaf_3ca7c0(Leaf3ca7c0& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x2ef6f88: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x000, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x008, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x010, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x018, 0x0000000000000000ull);
    // 0x2ef6fa8: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x020, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x028, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x030, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x038, 0x0000000000000000ull);
    // 0x2ef6fc8: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x040, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x048, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x050, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x058, 0x0000000000000000ull);
}

}  // namespace sotc
