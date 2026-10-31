#include "init_531d60_post_base_mov_rbx_rax_532b1e_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRbxRax_532b1e_U1Result init_531d60_post_base_mov_rbx_rax_532b1e_u1(
    const Init531d60PostBaseMovRbxRax_532b1e_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRbxRax_532b1e_U1Result out{};

    void* rbx = const_cast<void*>(cells.rax_in);
    out.rbx = rbx;
    if (cells.rbx_out != nullptr) {
        *cells.rbx_out = rbx;
    }
    ++out.mov_rbx_rax_count;

    // Next: call 0x54dc20 @ 0x532b21 — peeled as registry_body unit E553.
    out.ok = (out.mov_rbx_rax_count == 1);
    return out;
}

}  // namespace sotc
