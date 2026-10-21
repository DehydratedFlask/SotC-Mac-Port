#include "leaf_8a0440.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_leaf_8a0440(Leaf8a0440& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x3294f40: 16-byte store: 4 x 0xdeadbeef
    put_u64(g.cells.data() + 0x000, 0xdeadbeefdeadbeefull);
    put_u64(g.cells.data() + 0x008, 0xdeadbeefdeadbeefull);
    // 0x3294f50: 16-byte store: 4 x 0x206f85b3
    put_u64(g.cells.data() + 0x010, 0x206f85b3206f85b3ull);
    put_u64(g.cells.data() + 0x018, 0x206f85b3206f85b3ull);
    // 0x3294f60: 16-byte store: 4 x 0x5768b525
    put_u64(g.cells.data() + 0x020, 0x5768b5255768b525ull);
    put_u64(g.cells.data() + 0x028, 0x5768b5255768b525ull);
}

}  // namespace sotc
