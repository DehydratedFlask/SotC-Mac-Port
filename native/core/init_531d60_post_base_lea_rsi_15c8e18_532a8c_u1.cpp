#include "init_531d60_post_base_lea_rsi_15c8e18_532a8c_u1.hpp"

namespace sotc {

Init531d60PostBaseLeaRsi15c8e18_532a8c_U1Result init_531d60_post_base_lea_rsi_15c8e18_532a8c_u1(
    const Init531d60PostBaseLeaRsi15c8e18_532a8c_U1Cells& cells) noexcept {
    Init531d60PostBaseLeaRsi15c8e18_532a8c_U1Result out{};

    if (cells.rsi_out != nullptr) {
        *cells.rsi_out = reinterpret_cast<void*>(
            static_cast<std::uintptr_t>(kInit531d60PostBaseLeaRsi15c8e18_532a8c_U1TargetRva));
    }
    ++out.lea_rip_count;

    // Next: mov qword [rsp+0x4A0],0 @ 0x532a93 — NOT modelled here.
    out.ok = (out.lea_rip_count == 1);
    return out;
}

}  // namespace sotc
