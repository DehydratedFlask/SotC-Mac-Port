#include "init_531d60_post_base_lea_rsp470_532a84_u1.hpp"

namespace sotc {

Init531d60PostBaseLeaRsp470_532a84_U1Result init_531d60_post_base_lea_rsp470_532a84_u1(
    const Init531d60PostBaseLeaRsp470_532a84_U1Cells& cells) noexcept {
    Init531d60PostBaseLeaRsp470_532a84_U1Result out{};

    if (cells.rdx_out != nullptr) {
        if (cells.stack_rsp != nullptr) {
            *cells.rdx_out = reinterpret_cast<void*>(
                reinterpret_cast<std::uintptr_t>(cells.stack_rsp) +
                static_cast<std::uintptr_t>(kInit531d60PostBaseLeaRsp470_532a84_U1Disp));
        } else {
            *cells.rdx_out = nullptr;
        }
    }
    ++out.lea_rsp_count;

    // Next: lea rsi,[0x15c8e18] @ 0x532a8c — NOT modelled here.
    out.ok = (out.lea_rsp_count == 1);
    return out;
}

}  // namespace sotc
