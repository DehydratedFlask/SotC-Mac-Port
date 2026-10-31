#include "init_531d60_post_base_mov_rdi_rbx_532aef_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRdiRbx_532aef_U1Result init_531d60_post_base_mov_rdi_rbx_532aef_u1(
    const Init531d60PostBaseMovRdiRbx_532aef_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRdiRbx_532aef_U1Result out{};

    void* rdi = const_cast<void*>(cells.rbx_in);
    out.rdi = rdi;
    if (cells.rdi_out != nullptr) {
        *cells.rdi_out = rdi;
    }
    ++out.mov_rdi_rbx_count;

    // Next: call [rax+0x18] @ 0x532af2 — see continue_e.
    out.ok = (out.mov_rdi_rbx_count == 1);
    return out;
}

}  // namespace sotc
