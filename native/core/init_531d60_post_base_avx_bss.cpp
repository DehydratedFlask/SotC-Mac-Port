#include "init_531d60_post_base_avx_bss.hpp"

#include <cstring>

namespace sotc {

Init531d60PostBaseAvxBssResult init_531d60_post_base_avx_bss(
    const Init531d60PostBaseAvxBssCells& cells) noexcept {
    Init531d60PostBaseAvxBssResult out{};

    if (cells.src_1b1dd40 == nullptr || cells.dst_1b1dd00 == nullptr ||
        cells.dst_1b1dd20 == nullptr) {
        return out;
    }

    // OBS: vmovups xmm0/xmm1 from [0x1b1dd40]; store to [0x1b1dd00] then [0x1b1dd20].
    // Both loads read the same 16-byte source; both stores write that lane.
    std::memcpy(cells.dst_1b1dd00, cells.src_1b1dd40, kInit531d60PostBaseAvxBssLaneBytes);
    ++out.store_count;
    std::memcpy(cells.dst_1b1dd20, cells.src_1b1dd40, kInit531d60PostBaseAvxBssLaneBytes);
    ++out.store_count;

    // Next: call 0x7f7220 @ 0x531e3c — NOT modelled here.
    out.ok = (out.store_count == 2);
    return out;
}

}  // namespace sotc
