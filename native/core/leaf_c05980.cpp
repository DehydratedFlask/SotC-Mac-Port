#include "leaf_c05980.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
void put_u8(std::uint8_t* dst, std::uint8_t v) noexcept { *dst = v; }
}  // namespace

void init_leaf_c05980(LeafC05980& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x33512a0: 1-byte store: 04
    put_u8(g.cells.data() + 0x010, 0x04u);
    // 0x33512a1: 1-byte store: 04
    put_u8(g.cells.data() + 0x011, 0x04u);
    // 0x3351290: 16-byte store: lanes ffffffff ffffffff ffffffff 00000000
    put_u64(g.cells.data() + 0x000, 0xffffffffffffffffull);
    put_u64(g.cells.data() + 0x008, 0x00000000ffffffffull);
}

}  // namespace sotc
