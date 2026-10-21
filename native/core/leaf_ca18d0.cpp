#include "leaf_ca18d0.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_leaf_ca18d0(LeafCa18d0& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x3351ab0: 16-byte store: lanes ffffffff ffffffff ffffffff 00000000
    put_u64(g.cells.data() + 0x000, 0xffffffffffffffffull);
    put_u64(g.cells.data() + 0x008, 0x00000000ffffffffull);
    // 0x3351af0: 16-byte store: 4 x 0x40000000 (2.0f splat)
    put_u64(g.cells.data() + 0x040, 0x4000000040000000ull);
    put_u64(g.cells.data() + 0x048, 0x4000000040000000ull);
    // 0x3351b00: 16-byte store: 4 x 0x33d6bf95 (1.0000000116860974e-07f splat)
    put_u64(g.cells.data() + 0x050, 0x33d6bf9533d6bf95ull);
    put_u64(g.cells.data() + 0x058, 0x33d6bf9533d6bf95ull);
    // 0x3351ac0: 16-byte store: 4 x 0x80000000
    put_u64(g.cells.data() + 0x010, 0x8000000080000000ull);
    put_u64(g.cells.data() + 0x018, 0x8000000080000000ull);
    // 0x3351ad0: 16-byte store: 4 x 0x3089705f (9.999999717180685e-10f splat)
    put_u64(g.cells.data() + 0x020, 0x3089705f3089705full);
    put_u64(g.cells.data() + 0x028, 0x3089705f3089705full);
    // 0x3351ae0: 16-byte store: 4 x 0x00000000
    put_u64(g.cells.data() + 0x030, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x038, 0x0000000000000000ull);
}

}  // namespace sotc
