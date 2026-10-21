#include "avx_d8fa40.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_avx_d8fa40(AvxD8fa40& g) noexcept {
    // Unguarded: the original rewrites these cells on every call.
    // 0x3352bb0: one 16-byte store (split into 8-byte native writes)
    put_u64(g.cells.data() + 0x000, 0xffffffffffffffffull);
    put_u64(g.cells.data() + 0x008, 0x00000000ffffffffull);
    // 0x3352bf0: one 16-byte store (split into 8-byte native writes)
    put_u64(g.cells.data() + 0x040, 0x4000000040000000ull);
    put_u64(g.cells.data() + 0x048, 0x4000000040000000ull);
    // 0x3352c00: one 16-byte store (split into 8-byte native writes)
    put_u64(g.cells.data() + 0x050, 0x33d6bf9533d6bf95ull);
    put_u64(g.cells.data() + 0x058, 0x33d6bf9533d6bf95ull);
    // 0x3352bc0: one 16-byte store (split into 8-byte native writes)
    put_u64(g.cells.data() + 0x010, 0x8000000080000000ull);
    put_u64(g.cells.data() + 0x018, 0x8000000080000000ull);
    // 0x3352bd0: one 16-byte store (split into 8-byte native writes)
    put_u64(g.cells.data() + 0x020, 0x3089705f3089705full);
    put_u64(g.cells.data() + 0x028, 0x3089705f3089705full);
    // 0x3352be0: one 16-byte store (split into 8-byte native writes)
    put_u64(g.cells.data() + 0x030, 0x0000000000000000ull);
    put_u64(g.cells.data() + 0x038, 0x0000000000000000ull);
}

}  // namespace sotc
