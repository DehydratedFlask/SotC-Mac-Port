#include "leaf_c0d380.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_leaf_c0d380(LeafC0d380& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x3351350: 8-byte store: lanes 00000020 00000000
    put_u64(g.cells.data() + 0x030, 0x0000000000000020ull);
    // 0x3351388: 8-byte store: lanes 00000020 00000000
    put_u64(g.cells.data() + 0x068, 0x0000000000000020ull);
    // 0x3351378: 16-byte store: lanes 00000020 00000000 00000020 00000000
    put_u64(g.cells.data() + 0x058, 0x0000000000000020ull);
    put_u64(g.cells.data() + 0x060, 0x0000000000000020ull);
    // 0x3351330: 32-byte store: lanes 00000020 00000000 00000020 00000000 00000018 00000000 00000018 00000000
    put_u64(g.cells.data() + 0x010, 0x0000000000000020ull);
    put_u64(g.cells.data() + 0x018, 0x0000000000000020ull);
    put_u64(g.cells.data() + 0x020, 0x0000000000000018ull);
    put_u64(g.cells.data() + 0x028, 0x0000000000000018ull);
    // 0x3351398: 8-byte store: lanes 00000018 00000000
    put_u64(g.cells.data() + 0x078, 0x0000000000000018ull);
    // 0x3351358: 8-byte store: lanes 00000018 00000000
    put_u64(g.cells.data() + 0x038, 0x0000000000000018ull);
    // 0x3351320: 16-byte store: lanes ffffffff ffffffff ffffffff 00000000
    put_u64(g.cells.data() + 0x000, 0xffffffffffffffffull);
    put_u64(g.cells.data() + 0x008, 0x00000000ffffffffull);
    // 0x33513b8: 8-byte store: lanes 00000018 00000000
    put_u64(g.cells.data() + 0x098, 0x0000000000000018ull);
}

}  // namespace sotc
