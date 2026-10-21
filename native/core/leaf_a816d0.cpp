#include "leaf_a816d0.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
void put_u32(std::uint8_t* dst, std::uint32_t v) noexcept {
    for (int i = 0; i != 4; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_leaf_a816d0(LeafA816d0& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x32fb21c: 4-byte store: 1 x 0x00000000
    put_u32(g.cells.data() + 0x05c, 0x00000000u);
    // 0x32fb1c9: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x009, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x011, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x019, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x021, 0x0000000000000000ull);
    // 0x32fb1c0: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x000, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x008, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x010, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x018, 0x0000000000000000ull);
    // 0x32fb1f9: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x039, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x041, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x049, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x051, 0x0000000000000000ull);
    // 0x32fb1ec: 32-byte store: 8 x 0x00000000
    put_u64(g.cells.data() + 0x02c, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x034, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x03c, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x044, 0x0000000000000000ull);
}

}  // namespace sotc
