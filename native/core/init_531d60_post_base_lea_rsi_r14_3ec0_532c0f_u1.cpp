#include "init_531d60_post_base_lea_rsi_r14_3ec0_532c0f_u1.hpp"

namespace sotc {

Init531d60PostBaseLeaRsiR14_3ec0_532c0f_U1Result init_531d60_post_base_lea_rsi_r14_3ec0_532c0f_u1(
    const Init531d60PostBaseLeaRsiR14_3ec0_532c0f_U1Cells& cells) noexcept {
    Init531d60PostBaseLeaRsiR14_3ec0_532c0f_U1Result out{};

    if (cells.r14_in != nullptr) {
        out.rsi = reinterpret_cast<void*>(
            reinterpret_cast<std::uintptr_t>(cells.r14_in) +
            static_cast<std::uintptr_t>(kInit531d60PostBaseLeaRsiR14_3ec0_532c0f_U1Disp));
    }
    if (cells.rsi_out != nullptr) {
        *cells.rsi_out = out.rsi;
    }
    ++out.lea_rsi_r14_3ec0_532c0f_count;

    // Next: E614 mov rax,[rdi] @ 0x532c16 — separate unit.
    out.ok = (out.lea_rsi_r14_3ec0_532c0f_count == 1);
    return out;
}

}  // namespace sotc
