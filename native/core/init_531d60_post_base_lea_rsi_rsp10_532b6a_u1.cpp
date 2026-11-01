#include "init_531d60_post_base_lea_rsi_rsp10_532b6a_u1.hpp"

namespace sotc {

Init531d60PostBaseLeaRsiRsp10_532b6a_U1Result init_531d60_post_base_lea_rsi_rsp10_532b6a_u1(
    const Init531d60PostBaseLeaRsiRsp10_532b6a_U1Cells& cells) noexcept {
    Init531d60PostBaseLeaRsiRsp10_532b6a_U1Result out{};

    if (cells.stack_rsp != nullptr) {
        out.rsi = reinterpret_cast<void*>(
            reinterpret_cast<std::uintptr_t>(cells.stack_rsp) +
            static_cast<std::uintptr_t>(kInit531d60PostBaseLeaRsiRsp10_532b6a_U1Disp));
    }
    if (cells.rsi_out != nullptr) {
        *cells.rsi_out = out.rsi;
    }
    ++out.lea_rsi_rsp10_count;

    // Next: mov rbx,[r13+0x10] @ 0x532b6f — not modelled here.
    out.ok = (out.lea_rsi_rsp10_count == 1);
    return out;
}

}  // namespace sotc
