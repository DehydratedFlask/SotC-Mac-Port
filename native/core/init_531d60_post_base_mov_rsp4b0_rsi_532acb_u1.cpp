#include "init_531d60_post_base_mov_rsp4b0_rsi_532acb_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRsp4b0_Rsi_532acb_U1Result init_531d60_post_base_mov_rsp4b0_rsi_532acb_u1(
    const Init531d60PostBaseMovRsp4b0_Rsi_532acb_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRsp4b0_Rsi_532acb_U1Result out{};

    if (cells.vtable_stack_out != nullptr) {
        void* val = cells.rsi_in;
        if (val == nullptr) {
            val = reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                kInit531d60PostBaseMovRsp4b0_Rsi_532acb_U1DefaultVtableRva));
        }
        *cells.vtable_stack_out = val;
    }
    ++out.mov_rsp4b0_rsi_count;

    // Next: lea rsi,[0x2efda50] @ 0x532ad3 — see continue_e.
    out.ok = (out.mov_rsp4b0_rsi_count == 1);
    return out;
}

}  // namespace sotc
