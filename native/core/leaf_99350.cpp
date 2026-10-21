#include "leaf_99350.hpp"

#include <cstring>

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
void put_u32(std::uint8_t* dst, std::uint32_t v) noexcept {
    for (int i = 0; i != 4; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
void put_u8(std::uint8_t* dst, std::uint8_t v) noexcept { *dst = v; }
}  // namespace

void init_leaf_99350(Leaf99350& g, const Leaf99350Inputs& in) noexcept {
    // Unguarded: the original rewrites these cells on every call, in this order. The body also
    // reloads the dword it stored at 0x2ed37a0 (0x00f2e009) and stores it again at 0x2ed374c
    // (0x993b8 / 0x9944e); that value is the constant written below.
    // 0x2ed37a0: 4-byte store: 00f2e009
    put_u32(g.cells.data() + 0x120, 0x00f2e009u);
    // 0x2ed3760: 4-byte store: 00000000
    put_u32(g.cells.data() + 0x0e0, 0x00000000u);
    // 0x2ed3764: 4-byte store: 00000000
    put_u32(g.cells.data() + 0x0e4, 0x00000000u);
    // 0x2ed376c: 1-byte store: 00
    put_u8(g.cells.data() + 0x0ec, 0x00u);
    // 0x2ed376d: 1-byte store: 00
    put_u8(g.cells.data() + 0x0ed, 0x00u);
    // 0x2ed376e: 1-byte store: 01
    put_u8(g.cells.data() + 0x0ee, 0x01u);
    // 0x2ed376f: 1-byte store: 01
    put_u8(g.cells.data() + 0x0ef, 0x01u);
    // 0x2ed3770: 1-byte store: 00
    put_u8(g.cells.data() + 0x0f0, 0x00u);
    // 0x2ed3771: 1-byte store: 00
    put_u8(g.cells.data() + 0x0f1, 0x00u);
    // 0x2ed3768: 4-byte store: 00000000
    put_u32(g.cells.data() + 0x0e8, 0x00000000u);
    // 0x2ed3680: 32-byte store: 00fac00a 00fac90a 00fac70c 00fac70c 00fac00c 00fac00c 00924704 00924704
    put_u64(g.cells.data() + 0x000, 0x00fac90a00fac00aull);
    put_u64(g.cells.data() + 0x008, 0x00fac70c00fac70cull);
    put_u64(g.cells.data() + 0x010, 0x00fac00c00fac00cull);
    put_u64(g.cells.data() + 0x018, 0x0092470400924704ull);
    // 0x2ed3778: 8-byte store of the address constant 0x2dd9f84 (supplied by the caller)
    put_u64(g.cells.data() + 0x0f8, in.anchor);
    // 0x2ed36a0: 32-byte store: 00004404 00004404 00924702 00924702 00004402 00004402 0002c405 0002c405
    put_u64(g.cells.data() + 0x020, 0x0000440400004404ull);
    put_u64(g.cells.data() + 0x028, 0x0092470200924702ull);
    put_u64(g.cells.data() + 0x030, 0x0000440200004402ull);
    put_u64(g.cells.data() + 0x038, 0x0002c4050002c405ull);
    // 0x2ed3790: 8-byte store of the run-time object bytes at 0x2dd9fb8
    std::memcpy(g.cells.data() + 0x110, in.object.data() + 0x10, 8);
    // 0x2ed36c0: 32-byte store: 00204704 00204704 00204704 00204704 00204702 00204702 00fac023 00fac923
    put_u64(g.cells.data() + 0x040, 0x0020470400204704ull);
    put_u64(g.cells.data() + 0x048, 0x0020470400204704ull);
    put_u64(g.cells.data() + 0x050, 0x0020470200204702ull);
    put_u64(g.cells.data() + 0x058, 0x00fac92300fac023ull);
    // 0x2ed36e0: 32-byte store: 00fac024 00fac924 00fac025 00fac925 00204026 00204126 0022c027 0022c127
    put_u64(g.cells.data() + 0x060, 0x00fac92400fac024ull);
    put_u64(g.cells.data() + 0x068, 0x00fac92500fac025ull);
    put_u64(g.cells.data() + 0x070, 0x0020412600204026ull);
    put_u64(g.cells.data() + 0x078, 0x0022c1270022c027ull);
    // 0x2ed3700: 32-byte store: 003ac028 003ac028 00fac70e 00fac70e 0024c001 0024c001 00fac009 00fac009
    put_u64(g.cells.data() + 0x080, 0x003ac028003ac028ull);
    put_u64(g.cells.data() + 0x088, 0x00fac70e00fac70eull);
    put_u64(g.cells.data() + 0x090, 0x0024c0010024c001ull);
    put_u64(g.cells.data() + 0x098, 0x00fac00900fac009ull);
    // 0x2ed3720: 32-byte store: 0022c705 0022c705 003ac706 003ac706 00004401 00004401 0022c70b 0022c70b
    put_u64(g.cells.data() + 0x0a0, 0x0022c7050022c705ull);
    put_u64(g.cells.data() + 0x0a8, 0x003ac706003ac706ull);
    put_u64(g.cells.data() + 0x0b0, 0x0000440100004401ull);
    put_u64(g.cells.data() + 0x0b8, 0x0022c70b0022c70bull);
    // 0x2ed3740: 4-byte store: 0002c40b
    put_u32(g.cells.data() + 0x0c0, 0x0002c40bu);
    // 0x2ed3744: 4-byte store: 0002c40b
    put_u32(g.cells.data() + 0x0c4, 0x0002c40bu);
    // 0x2ed3748: 4-byte store: 00f2e009
    put_u32(g.cells.data() + 0x0c8, 0x00f2e009u);
    // 0x2ed374c: 4-byte store: 00f2e009
    put_u32(g.cells.data() + 0x0cc, 0x00f2e009u);
    // 0x2ed3750: 16-byte store: 0022c003 0022c003 00fac40e 00fac40e
    put_u64(g.cells.data() + 0x0d0, 0x0022c0030022c003ull);
    put_u64(g.cells.data() + 0x0d8, 0x00fac40e00fac40eull);
    // 0x2ed3780: 16-byte store of the run-time object bytes at 0x2dd9fa8
    std::memcpy(g.cells.data() + 0x100, in.object.data() + 0x00, 16);
    // 0x2ed3798: 1-byte store: 00
    put_u8(g.cells.data() + 0x118, 0x00u);
}

}  // namespace sotc
