#include "init_531d60_post_base_mov_r14_rax_532b16_u1.hpp"

namespace sotc {

Init531d60PostBaseMovR14Rax_532b16_U1Result init_531d60_post_base_mov_r14_rax_532b16_u1(
    const Init531d60PostBaseMovR14Rax_532b16_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR14Rax_532b16_U1Result out{};

    void* r14 = const_cast<void*>(cells.rax_in);
    out.r14 = r14;
    if (cells.r14_out != nullptr) {
        *cells.r14_out = r14;
    }
    ++out.mov_r14_rax_count;

    // Next: registry body @ 0x532b19 — not modelled in this unit.
    out.ok = (out.mov_r14_rax_count == 1);
    return out;
}

}  // namespace sotc
