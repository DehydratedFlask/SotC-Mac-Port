#include "init_531d60_post_base_mov_r15_rax_532b26_u1.hpp"

namespace sotc {

Init531d60PostBaseMovR15Rax_532b26_U1Result init_531d60_post_base_mov_r15_rax_532b26_u1(
    const Init531d60PostBaseMovR15Rax_532b26_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR15Rax_532b26_U1Result out{};

    out.r15 = cells.rax_in;
    if (cells.r15_out != nullptr) {
        *cells.r15_out = out.r15;
    }
    ++out.mov_r15_rax_count;

    // Next: mov rsi,rbx / fill setup @ 0x532b29 — not modelled in this unit.
    out.ok = (out.mov_r15_rax_count == 1);
    return out;
}

}  // namespace sotc
