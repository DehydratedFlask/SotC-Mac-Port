#include "leaf_8f3d90.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_leaf_8f3d90(Leaf8f3d90& g) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order.
    // 0x3294f90: 16-byte store: lanes ffff0100 ffff0302 ffff0504 ffff0706
    put_u64(g.cells.data() + 0x000, 0xffff0302ffff0100ull);
    put_u64(g.cells.data() + 0x008, 0xffff0706ffff0504ull);
    // 0x3294fa0: 16-byte store: lanes 0f0e0d0c ffff0100 ffff0302 ffff0504
    put_u64(g.cells.data() + 0x010, 0xffff01000f0e0d0cull);
    put_u64(g.cells.data() + 0x018, 0xffff0504ffff0302ull);
    // 0x3294fb0: 16-byte store: lanes ffff0100 ffff0100 ffff0302 ffff0504
    put_u64(g.cells.data() + 0x020, 0xffff0100ffff0100ull);
    put_u64(g.cells.data() + 0x028, 0xffff0504ffff0302ull);
    // 0x3294fc0: 16-byte store: lanes 0f0e0d0c 0f0e0d0c ffff0100 ffff0302
    put_u64(g.cells.data() + 0x030, 0x0f0e0d0c0f0e0d0cull);
    put_u64(g.cells.data() + 0x038, 0xffff0302ffff0100ull);
    // 0x3294fd0: 16-byte store: lanes ffff0100 ffff0302 ffff0302 ffff0504
    put_u64(g.cells.data() + 0x040, 0xffff0302ffff0100ull);
    put_u64(g.cells.data() + 0x048, 0xffff0504ffff0302ull);
    // 0x3294fe0: 16-byte store: lanes 0f0e0d0c ffff0100 ffff0100 ffff0302
    put_u64(g.cells.data() + 0x050, 0xffff01000f0e0d0cull);
    put_u64(g.cells.data() + 0x058, 0xffff0302ffff0100ull);
    // 0x3294ff0: 16-byte store: lanes ffff0100 ffff0100 ffff0100 ffff0302
    put_u64(g.cells.data() + 0x060, 0xffff0100ffff0100ull);
    put_u64(g.cells.data() + 0x068, 0xffff0302ffff0100ull);
    // 0x3295000: 16-byte store: lanes 0f0e0d0c 0f0e0d0c 0f0e0d0c ffff0100
    put_u64(g.cells.data() + 0x070, 0x0f0e0d0c0f0e0d0cull);
    put_u64(g.cells.data() + 0x078, 0xffff01000f0e0d0cull);
    // 0x3295010: 16-byte store: lanes ffff0100 ffff0302 ffff0504 ffff0504
    put_u64(g.cells.data() + 0x080, 0xffff0302ffff0100ull);
    put_u64(g.cells.data() + 0x088, 0xffff0504ffff0504ull);
    // 0x3295020: 16-byte store: lanes 0f0e0d0c ffff0100 ffff0302 ffff0302
    put_u64(g.cells.data() + 0x090, 0xffff01000f0e0d0cull);
    put_u64(g.cells.data() + 0x098, 0xffff0302ffff0302ull);
    // 0x3295030: 16-byte store: lanes ffff0100 ffff0100 ffff0302 ffff0302
    put_u64(g.cells.data() + 0x0a0, 0xffff0100ffff0100ull);
    put_u64(g.cells.data() + 0x0a8, 0xffff0302ffff0302ull);
    // 0x3295040: 16-byte store: lanes 0f0e0d0c 0f0e0d0c ffff0100 ffff0100
    put_u64(g.cells.data() + 0x0b0, 0x0f0e0d0c0f0e0d0cull);
    put_u64(g.cells.data() + 0x0b8, 0xffff0100ffff0100ull);
    // 0x3295050: 16-byte store: lanes ffff0100 ffff0302 ffff0302 ffff0302
    put_u64(g.cells.data() + 0x0c0, 0xffff0302ffff0100ull);
    put_u64(g.cells.data() + 0x0c8, 0xffff0302ffff0302ull);
    // 0x3295060: 16-byte store: lanes 0f0e0d0c ffff0100 ffff0100 ffff0100
    put_u64(g.cells.data() + 0x0d0, 0xffff01000f0e0d0cull);
    put_u64(g.cells.data() + 0x0d8, 0xffff0100ffff0100ull);
    // 0x3295070: 16-byte store: 4 x 0xffff0100
    put_u64(g.cells.data() + 0x0e0, 0xffff0100ffff0100ull);
    put_u64(g.cells.data() + 0x0e8, 0xffff0100ffff0100ull);
    // 0x3295080: 16-byte store: 4 x 0x0f0e0d0c
    put_u64(g.cells.data() + 0x0f0, 0x0f0e0d0c0f0e0d0cull);
    put_u64(g.cells.data() + 0x0f8, 0x0f0e0d0c0f0e0d0cull);
}

}  // namespace sotc
