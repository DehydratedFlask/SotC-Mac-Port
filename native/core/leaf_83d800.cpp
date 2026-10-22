#include "leaf_83d800.hpp"

namespace sotc {
namespace {
void put_u32(std::uint8_t* dst, std::uint32_t v) noexcept {
    for (int i = 0; i != 4; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_leaf_83d800(Leaf83d800& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x3293400: 4-byte store: 00000000
    put_u32(g.cells.data() + 0x000, 0x00000000u);
    // 0x3293458: 4-byte store: ffffffff
    put_u32(g.cells.data() + 0x058, 0xffffffffu);
    // 0x3293460: 4-byte store: 00000000
    put_u32(g.cells.data() + 0x060, 0x00000000u);
    // 0x32934b8: 4-byte store: ffffffff
    put_u32(g.cells.data() + 0x0b8, 0xffffffffu);
    // 0x32934c0: 4-byte store: 00000000
    put_u32(g.cells.data() + 0x0c0, 0x00000000u);
    // 0x3293518: 4-byte store: ffffffff
    put_u32(g.cells.data() + 0x118, 0xffffffffu);
}

}  // namespace sotc
