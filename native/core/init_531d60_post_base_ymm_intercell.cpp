#include "init_531d60_post_base_ymm_intercell.hpp"

#include <cstring>

namespace sotc {

Init531d60PostBaseYmmIntercellResult init_531d60_post_base_ymm_intercell(
    const Init531d60PostBaseYmmIntercellCells& cells) noexcept {
    Init531d60PostBaseYmmIntercellResult out{};

    if (cells.src_1604278 == nullptr || cells.src_16042dc == nullptr ||
        cells.dst_16042a8 == nullptr || cells.dst_160430c == nullptr) {
        return out;
    }

    // OBS: overlapping ymm stores to [rcx=0x16042a8] from [rsi=0x1604278]
    // resolve to a contiguous 0x30-byte copy.
    std::memcpy(cells.dst_16042a8, cells.src_1604278,
                kInit531d60PostBaseYmmIntercellBlockBytes);
    ++out.store_count;

    // OBS: overlapping ymm stores to [rsi=0x160430c] from [rdi=0x16042dc]
    // resolve to a contiguous 0x30-byte copy.
    std::memcpy(cells.dst_160430c, cells.src_16042dc,
                kInit531d60PostBaseYmmIntercellBlockBytes);
    ++out.store_count;

    // Next: vmovups xmm from 0x1014be0 @ 0x531f07 — E459; not this fragment.
    // Later: call 0x26800 / 0xf590 — NOT modelled here.
    out.ok = (out.store_count == 2);
    return out;
}

}  // namespace sotc
