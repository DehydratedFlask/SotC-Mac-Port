#include "init_531d60_post_base_lea_rbx_rsp490_532aaa_u1.hpp"

namespace sotc {

Init531d60PostBaseLeaRbxRsp490_532aaa_U1Result init_531d60_post_base_lea_rbx_rsp490_532aaa_u1(
    const Init531d60PostBaseLeaRbxRsp490_532aaa_U1Cells& cells) noexcept {
    Init531d60PostBaseLeaRbxRsp490_532aaa_U1Result out{};

    if (cells.rbx_out != nullptr) {
        if (cells.stack_rsp != nullptr) {
            *cells.rbx_out = reinterpret_cast<void*>(
                reinterpret_cast<std::uintptr_t>(cells.stack_rsp) +
                static_cast<std::uintptr_t>(kInit531d60PostBaseLeaRbxRsp490_532aaa_U1Disp));
        } else {
            *cells.rbx_out = nullptr;
        }
    }
    ++out.lea_rsp_count;

    // Next: mov [rsp+0x470],rdx @ 0x532ab2 — see continue_e.
    out.ok = (out.lea_rsp_count == 1);
    return out;
}

}  // namespace sotc
