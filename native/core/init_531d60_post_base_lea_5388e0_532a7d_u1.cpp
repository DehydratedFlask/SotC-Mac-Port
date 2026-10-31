#include "init_531d60_post_base_lea_5388e0_532a7d_u1.hpp"

namespace sotc {

Init531d60PostBaseLea5388e0_532a7d_U1Result init_531d60_post_base_lea_5388e0_532a7d_u1(
    const Init531d60PostBaseLea5388e0_532a7d_U1Cells& cells) noexcept {
    Init531d60PostBaseLea5388e0_532a7d_U1Result out{};

    if (cells.rax_out != nullptr) {
        *cells.rax_out = reinterpret_cast<void*>(
            static_cast<std::uintptr_t>(kInit531d60PostBaseLea5388e0_532a7d_U1TargetRva));
    }
    ++out.lea_rip_count;

    // Next: lea rdx,[rsp+0x470] @ 0x532a84 — NOT modelled here.
    out.ok = (out.lea_rip_count == 1);
    return out;
}

}  // namespace sotc
