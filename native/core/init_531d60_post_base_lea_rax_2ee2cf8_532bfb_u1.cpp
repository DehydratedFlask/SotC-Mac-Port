#include "init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.hpp"

namespace sotc {

Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Result init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1(
    const Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Cells& cells) noexcept {
    Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Result out{};

    out.rax = reinterpret_cast<void*>(
        static_cast<std::uintptr_t>(kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1TargetRva));
    if (cells.rax_out != nullptr) {
        *cells.rax_out = out.rax;
    }
    ++out.lea_rax_2ee2cf8_532bfb_count;

    // Next: mov r14,[rsp+0x20] @ 0x532c02 — not modelled here.
    out.ok = (out.lea_rax_2ee2cf8_532bfb_count == 1);
    return out;
}

}  // namespace sotc
