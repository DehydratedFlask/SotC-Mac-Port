#include "leaf_c675f0.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_leaf_c675f0(LeafC675f0& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x3351770: 16-byte store: 4 x 0x00000000
    put_u64(g.cells.data() + 0x010, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x018, 0x0000000000000000ull);
    // 0x3351760: 16-byte store: lanes ffffffff ffffffff ffffffff 00000000
    put_u64(g.cells.data() + 0x000, 0xffffffffffffffffull);
    put_u64(g.cells.data() + 0x008, 0x00000000ffffffffull);
    // 0x3351780: 16-byte store: lanes ffffffff 00000000 00000000 00000000
    put_u64(g.cells.data() + 0x020, 0x00000000ffffffffull);
    put_u64(g.cells.data() + 0x028, 0x0000000000000000ull);
    // 0x3351790: 16-byte store: lanes 00000000 ffffffff 00000000 00000000
    put_u64(g.cells.data() + 0x030, 0xffffffff00000000ull);
    put_u64(g.cells.data() + 0x038, 0x0000000000000000ull);
    // 0x33517a0: 16-byte store: lanes ffffffff ffffffff 00000000 00000000
    put_u64(g.cells.data() + 0x040, 0xffffffffffffffffull);
    put_u64(g.cells.data() + 0x048, 0x0000000000000000ull);
    // 0x33517b0: 16-byte store: lanes 00000000 00000000 ffffffff 00000000
    put_u64(g.cells.data() + 0x050, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x058, 0x00000000ffffffffull);
    // 0x33517c0: 16-byte store: lanes ffffffff 00000000 ffffffff 00000000
    put_u64(g.cells.data() + 0x060, 0x00000000ffffffffull);
    put_u64(g.cells.data() + 0x068, 0x00000000ffffffffull);
    // 0x33517d0: 16-byte store: lanes 00000000 ffffffff ffffffff 00000000
    put_u64(g.cells.data() + 0x070, 0xffffffff00000000ull);
    put_u64(g.cells.data() + 0x078, 0x00000000ffffffffull);
    // 0x33517e0: 16-byte store: lanes ffffffff ffffffff ffffffff 00000000
    put_u64(g.cells.data() + 0x080, 0xffffffffffffffffull);
    put_u64(g.cells.data() + 0x088, 0x00000000ffffffffull);
}

}  // namespace sotc
