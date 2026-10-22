#include "leaf_e71990.hpp"

namespace sotc {
namespace {
void put_u64(std::uint8_t* dst, std::uint64_t v) noexcept {
    for (int i = 0; i != 8; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
void put_u32(std::uint8_t* dst, std::uint32_t v) noexcept {
    for (int i = 0; i != 4; ++i) dst[i] = static_cast<std::uint8_t>(v >> (8 * i));
}
}  // namespace

void init_leaf_e71990(LeafE71990& g, const LeafE71990Inputs& in) noexcept {
    // Unguarded: the original rewrites these cells on every call, record by record (pointer store,
    // then hash store). The 4 bytes at +0x0c of each record are never written.
    for (std::size_t k = 0; k != kLeafE71990Records; ++k) {
        put_u64(g.cells.data() + 0x10 * k + 0x00, in.name_address[k]);
        put_u32(g.cells.data() + 0x10 * k + 0x08, kLeafE71990Hash[k]);
    }
}

}  // namespace sotc
