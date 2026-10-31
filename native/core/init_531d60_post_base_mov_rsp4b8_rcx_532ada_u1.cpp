#include "init_531d60_post_base_mov_rsp4b8_rcx_532ada_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRsp4b8_Rcx_532ada_U1Result init_531d60_post_base_mov_rsp4b8_rcx_532ada_u1(
    const Init531d60PostBaseMovRsp4b8_Rcx_532ada_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRsp4b8_Rcx_532ada_U1Result out{};

    if (cells.thunk_out != nullptr) {
        void* val = cells.rcx_in;
        if (val == nullptr) {
            val = reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                kInit531d60PostBaseMovRsp4b8_Rcx_532ada_U1DefaultThunkRva));
        }
        *cells.thunk_out = val;
    }
    ++out.mov_rsp4b8_rcx_count;

    // Next: call 0xa54920 @ 0x532ae2 — see continue_e.
    out.ok = (out.mov_rsp4b8_rcx_count == 1);
    return out;
}

}  // namespace sotc
