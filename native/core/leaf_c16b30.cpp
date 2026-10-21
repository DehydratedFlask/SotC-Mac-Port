#include "leaf_c16b30.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_leaf_c16b30(LeafC16b30& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x33513f0: 16-byte store: lanes ffffffff ffffffff ffffffff 00000000
    put_u64(g.cells.data() + 0x000, 0xffffffffffffffffull);
    put_u64(g.cells.data() + 0x008, 0x00000000ffffffffull);
    // 0x3351510: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x120, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x128, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x130, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x138, 0x0000000000000000ull);
    // 0x3351500: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x110, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x118, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x120, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x128, 0x0000000000000000ull);
    // 0x33514e0: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x0f0, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x0f8, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x100, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x108, 0x0000000000000000ull);
    // 0x33514c0: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x0d0, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x0d8, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x0e0, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x0e8, 0x0000000000000000ull);
    // 0x33514a0: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x0b0, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x0b8, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x0c0, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x0c8, 0x0000000000000000ull);
    // 0x3351480: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x090, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x098, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x0a0, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x0a8, 0x0000000000000000ull);
    // 0x3351460: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x070, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x078, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x080, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x088, 0x0000000000000000ull);
    // 0x3351440: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x050, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x058, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x060, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x068, 0x0000000000000000ull);
    // 0x3351420: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x030, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x038, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x040, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x048, 0x0000000000000000ull);
    // 0x3351400: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x010, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x018, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x020, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x028, 0x0000000000000000ull);
    // 0x3351430: 16-byte store: lanes 00000018 00000000 00000018 00000000
    put_u64(g.cells.data() + 0x040, 0x0000000000000018ull);
    put_u64(g.cells.data() + 0x048, 0x0000000000000018ull);
    // 0x3351468: 8-byte store: lanes 00000018 00000000
    put_u64(g.cells.data() + 0x078, 0x0000000000000018ull);
    // 0x3351480: 8-byte store: lanes 00000018 00000000
    put_u64(g.cells.data() + 0x090, 0x0000000000000018ull);
    // 0x3351458: 16-byte store: lanes 00000018 00000000 00000018 00000000
    put_u64(g.cells.data() + 0x068, 0x0000000000000018ull);
    put_u64(g.cells.data() + 0x070, 0x0000000000000018ull);
    // 0x33514c8: 16-byte store: lanes 00000060 00000000 00000060 00000000
    put_u64(g.cells.data() + 0x0d8, 0x0000000000000060ull);
    put_u64(g.cells.data() + 0x0e0, 0x0000000000000060ull);
    // 0x3351500: 8-byte store: lanes 00000028 00000000
    put_u64(g.cells.data() + 0x110, 0x0000000000000028ull);
    // 0x3351518: 8-byte store: lanes 00000060 00000000
    put_u64(g.cells.data() + 0x128, 0x0000000000000060ull);
    // 0x33514f0: 16-byte store: lanes 00000028 00000000 00000028 00000000
    put_u64(g.cells.data() + 0x100, 0x0000000000000028ull);
    put_u64(g.cells.data() + 0x108, 0x0000000000000028ull);
}

}  // namespace sotc
