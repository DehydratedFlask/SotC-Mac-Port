#include "init_531d60_post_base_mov_rdi_r13_532a9f_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRdiR13_532a9f_U1Result init_531d60_post_base_mov_rdi_r13_532a9f_u1(
    const Init531d60PostBaseMovRdiR13_532a9f_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRdiR13_532a9f_U1Result out{};

    void* parent = const_cast<void*>(cells.incoming_r13);
    out.parent = parent;
    if (cells.rdi_out != nullptr) {
        *cells.rdi_out = parent;
    }
    ++out.mov_rdi_r13_count;

    // Next: lea rcx,[0x538870] @ 0x532aa3 — NOT modelled here.
    out.ok = (out.mov_rdi_r13_count == 1);
    return out;
}

}  // namespace sotc
