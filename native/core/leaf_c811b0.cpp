#include "leaf_c811b0.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_leaf_c811b0(LeafC811b0& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x3351a00: 16-byte store: lanes ffffffff ffffffff ffffffff 00000000
    put_u64(g.cells.data() + 0x000, 0xffffffffffffffffull);
    put_u64(g.cells.data() + 0x008, 0x00000000ffffffffull);
    // 0x3351a10: 16-byte store: 4 x 0x80000000
    put_u64(g.cells.data() + 0x010, 0x8000000080000000ull);
    put_u64(g.cells.data() + 0x018, 0x8000000080000000ull);
}

}  // namespace sotc
