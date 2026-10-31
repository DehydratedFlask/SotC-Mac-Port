#include "init_531d60_post_base_lea_538870_532aa3_u1.hpp"

namespace sotc {

Init531d60PostBaseLea538870_532aa3_U1Result init_531d60_post_base_lea_538870_532aa3_u1(
    const Init531d60PostBaseLea538870_532aa3_U1Cells& cells) noexcept {
    Init531d60PostBaseLea538870_532aa3_U1Result out{};

    if (cells.rcx_out != nullptr) {
        *cells.rcx_out = reinterpret_cast<void*>(
            static_cast<std::uintptr_t>(kInit531d60PostBaseLea538870_532aa3_U1TargetRva));
    }
    ++out.lea_rip_count;

    // Next: lea rbx,[rsp+0x490] @ 0x532aaa — see continue_e.
    out.ok = (out.lea_rip_count == 1);
    return out;
}

}  // namespace sotc
