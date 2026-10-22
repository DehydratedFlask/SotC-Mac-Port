#include "init_531d60_post_base_ymm_reload.hpp"

#include <cstring>

namespace sotc {

Init531d60PostBaseYmmReloadResult init_531d60_post_base_ymm_reload(
    const Init531d60PostBaseYmmReloadCells& cells) noexcept {
    Init531d60PostBaseYmmReloadResult out{};

    if (cells.src_160430c == nullptr || cells.dst_160433c == nullptr) {
        return out;
    }

    // OBS: overlapping ymm loads from [rsi=0x160430c] and stores to
    // [rdi=0x160433c] resolve to a contiguous 0x30-byte copy.
    // Intervening CP10/stack prep is NOT modelled.
    std::memcpy(cells.dst_160433c, cells.src_160430c,
                kInit531d60PostBaseYmmReloadBlockBytes);
    ++out.store_count;

    // Next: vxorps / call 0x26800+0xf590 @ 0x531f71 — E461; not this fragment.
    out.ok = (out.store_count == 1);
    return out;
}

}  // namespace sotc
