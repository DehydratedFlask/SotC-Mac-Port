#include "init_531d60_post_base_lea_rbx_2efd160_532b33_u1.hpp"

namespace sotc {

Init531d60PostBaseLeaRbx2efd160_532b33_U1Result init_531d60_post_base_lea_rbx_2efd160_532b33_u1(
    const Init531d60PostBaseLeaRbx2efd160_532b33_U1Cells& cells) noexcept {
    Init531d60PostBaseLeaRbx2efd160_532b33_U1Result out{};

    out.rbx = reinterpret_cast<void*>(
        static_cast<std::uintptr_t>(kInit531d60PostBaseLeaRbx2efd160_532b33_U1TargetRva));
    if (cells.rbx_out != nullptr) {
        *cells.rbx_out = out.rbx;
    }
    ++out.lea_rip_count;

    // Next: mov r13,[rip+disp] @ 0x532b3a — not modelled in this unit.
    out.ok = (out.lea_rip_count == 1);
    return out;
}

}  // namespace sotc
