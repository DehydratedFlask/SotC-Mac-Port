#include "init_531d60_post_base_lea_rdi_rbx_28_532b84_u1.hpp"

namespace sotc {

Init531d60PostBaseLeaRdiRbx_28_532b84_U1Result init_531d60_post_base_lea_rdi_rbx_28_532b84_u1(
    const Init531d60PostBaseLeaRdiRbx_28_532b84_U1Cells& cells) noexcept {
    Init531d60PostBaseLeaRdiRbx_28_532b84_U1Result out{};

    if (cells.rbx_base != nullptr) {
        out.rdi = reinterpret_cast<void*>(
            reinterpret_cast<std::uintptr_t>(cells.rbx_base) +
            static_cast<std::uintptr_t>(kInit531d60PostBaseLeaRdiRbx_28_532b84_U1Disp));
    }
    if (cells.rdi_out != nullptr) {
        *cells.rdi_out = out.rdi;
    }
    ++out.lea_rdi_rbx_28_count;

    // Next: call [rax+0x18] @ 0x532b88 — not modelled here.
    out.ok = (out.lea_rdi_rbx_28_count == 1);
    return out;
}

}  // namespace sotc
