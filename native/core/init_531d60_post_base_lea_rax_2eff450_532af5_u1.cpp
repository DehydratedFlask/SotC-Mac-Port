#include "init_531d60_post_base_lea_rax_2eff450_532af5_u1.hpp"

namespace sotc {

Init531d60PostBaseLeaRax2eff450_532af5_U1Result init_531d60_post_base_lea_rax_2eff450_532af5_u1(
    const Init531d60PostBaseLeaRax2eff450_532af5_U1Cells& cells) noexcept {
    Init531d60PostBaseLeaRax2eff450_532af5_U1Result out{};

    if (cells.rax_out != nullptr) {
        *cells.rax_out = reinterpret_cast<void*>(
            static_cast<std::uintptr_t>(kInit531d60PostBaseLeaRax2eff450_532af5_U1TargetRva));
    }
    ++out.lea_rip_count;

    // Next: xor esi,esi @ 0x532afc — see continue_e.
    out.ok = (out.lea_rip_count == 1);
    return out;
}

}  // namespace sotc
