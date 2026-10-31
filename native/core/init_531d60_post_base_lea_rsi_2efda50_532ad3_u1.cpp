#include "init_531d60_post_base_lea_rsi_2efda50_532ad3_u1.hpp"

namespace sotc {

Init531d60PostBaseLeaRsi2efda50_532ad3_U1Result init_531d60_post_base_lea_rsi_2efda50_532ad3_u1(
    const Init531d60PostBaseLeaRsi2efda50_532ad3_U1Cells& cells) noexcept {
    Init531d60PostBaseLeaRsi2efda50_532ad3_U1Result out{};

    if (cells.rsi_out != nullptr) {
        *cells.rsi_out = reinterpret_cast<void*>(
            static_cast<std::uintptr_t>(kInit531d60PostBaseLeaRsi2efda50_532ad3_U1TargetRva));
    }
    ++out.lea_rip_count;

    // Next: mov [rsp+0x4B8],rcx @ 0x532ada — see mov_rsp4b8_rcx_532ada_u1.
    out.ok = (out.lea_rip_count == 1);
    return out;
}

}  // namespace sotc
