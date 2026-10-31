#include "init_531d60_post_base_mov_rsp488_rax_532ac3_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRsp488Rax_532ac3_U1Result init_531d60_post_base_mov_rsp488_rax_532ac3_u1(
    const Init531d60PostBaseMovRsp488Rax_532ac3_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRsp488Rax_532ac3_U1Result out{};

    if (cells.thunk_out != nullptr) {
        void* val = cells.rax_in;
        if (val == nullptr) {
            val = reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                kInit531d60PostBaseMovRsp488Rax_532ac3_U1DefaultThunkRva));
        }
        *cells.thunk_out = val;
    }
    ++out.mov_rsp488_rax_count;

    // Next: mov [rsp+0x4B0],rsi @ 0x532acb — see mov_rsp4b0_rsi_532acb_u1.
    out.ok = (out.mov_rsp488_rax_count == 1);
    return out;
}

}  // namespace sotc
