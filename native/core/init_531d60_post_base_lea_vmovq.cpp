#include "init_531d60_post_base_lea_vmovq.hpp"

#include <cstring>

namespace sotc {

Init531d60PostBaseLeaVmovqResult init_531d60_post_base_lea_vmovq(
    const Init531d60PostBaseLeaVmovqCells& cells) noexcept {
    Init531d60PostBaseLeaVmovqResult out{};

    if (cells.src_15ffa38 == nullptr || cells.dst_xmm0 == nullptr) {
        return out;
    }

    // OBS: lea rbx, [0x32f79a8] — address pin (optional host stand-in).
    if (cells.rbx_out != nullptr && cells.rbx_cell != nullptr) {
        *cells.rbx_out = cells.rbx_cell;
    }
    ++out.lea_count;

    // OBS: vmovq xmm0, [0x15ffa38]
    std::memcpy(cells.dst_xmm0, cells.src_15ffa38, kInit531d60PostBaseLeaVmovqLaneBytes);
    ++out.load_count;

    // Next: lea rsi / stack prep → call 0x532ca0 — NOT modelled here.
    out.ok = (out.lea_count == 1) && (out.load_count == 1);
    return out;
}

}  // namespace sotc
