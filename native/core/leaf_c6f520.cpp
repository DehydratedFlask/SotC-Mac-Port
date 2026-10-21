#include "leaf_c6f520.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_leaf_c6f520(LeafC6f520& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x3351940: 32-byte store: lanes 3f800000 00000000 00000000 00000000 3f800000 00000000 00000000 00000000
    put_u64(g.cells.data() + 0x010, 0x000000003f800000ull);
    put_u64(g.cells.data() + 0x018, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x020, 0x000000003f800000ull);
    put_u64(g.cells.data() + 0x028, 0x0000000000000000ull);
    // 0x3351960: 32-byte store: lanes 3f800000 bf800000 00000000 00000000 00000000 bf800000 00000000 00000000
    put_u64(g.cells.data() + 0x030, 0xbf8000003f800000ull);
    put_u64(g.cells.data() + 0x038, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x040, 0xbf80000000000000ull);
    put_u64(g.cells.data() + 0x048, 0x0000000000000000ull);
    // 0x3351930: 16-byte store: lanes ffffffff ffffffff ffffffff 00000000
    put_u64(g.cells.data() + 0x000, 0xffffffffffffffffull);
    put_u64(g.cells.data() + 0x008, 0x00000000ffffffffull);
    // 0x3351980: 8-byte store: lanes 00000000 bf800000
    put_u64(g.cells.data() + 0x050, 0xbf80000000000000ull);
}

}  // namespace sotc
