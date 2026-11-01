#include "init_531d60_post_base_mov_rdi_rbx_532b9d_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRdiRbx_532b9d_U1Result init_531d60_post_base_mov_rdi_rbx_532b9d_u1(
    const Init531d60PostBaseMovRdiRbx_532b9d_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRdiRbx_532b9d_U1Result out{};

    void* rdi = const_cast<void*>(cells.rbx_in);
    out.rdi = rdi;
    if (cells.rdi_out != nullptr) {
        *cells.rdi_out = rdi;
    }
    ++out.mov_rdi_rbx_532b9d_count;

    // Next: call 0x170c0 @ 0x532ba0 — not modelled here.
    out.ok = (out.mov_rdi_rbx_532b9d_count == 1);
    return out;
}

}  // namespace sotc
