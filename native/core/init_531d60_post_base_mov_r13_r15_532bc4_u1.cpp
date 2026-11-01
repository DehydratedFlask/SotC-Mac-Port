#include "init_531d60_post_base_mov_r13_r15_532bc4_u1.hpp"

namespace sotc {

Init531d60PostBaseMovR13R15_532bc4_U1Result init_531d60_post_base_mov_r13_r15_532bc4_u1(
    const Init531d60PostBaseMovR13R15_532bc4_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR13R15_532bc4_U1Result out{};

    out.r13 = cells.r15_in;
    if (cells.r13_out != nullptr) {
        *cells.r13_out = out.r13;
    }
    ++out.mov_r13_r15_count;

    // Next: E596 call 0x170c0 @ 0x532bc7 — not modelled here.
    out.ok = (out.mov_r13_r15_count == 1);
    return out;
}

}  // namespace sotc
