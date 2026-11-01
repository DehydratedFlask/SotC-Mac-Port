#include "init_531d60_post_base_mov_rdi_r12_532bf0_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRdiR12_532bf0_U1Result init_531d60_post_base_mov_rdi_r12_532bf0_u1(
    const Init531d60PostBaseMovRdiR12_532bf0_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRdiR12_532bf0_U1Result out{};

    void* rdi = const_cast<void*>(cells.r12_in);
    out.rdi = rdi;
    if (cells.rdi_out != nullptr) {
        *cells.rdi_out = rdi;
    }
    ++out.mov_rdi_r12_532bf0_count;

    // Next: E607 call 0x170b0 @ 0x532bf3 — not modelled here.
    out.ok = (out.mov_rdi_r12_532bf0_count == 1);
    return out;
}

}  // namespace sotc
