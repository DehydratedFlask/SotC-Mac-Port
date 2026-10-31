#include "init_531d60_post_base_mov_rsp470_rdx_532ab2_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRsp470Rdx_532ab2_U1Result init_531d60_post_base_mov_rsp470_rdx_532ab2_u1(
    const Init531d60PostBaseMovRsp470Rdx_532ab2_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRsp470Rdx_532ab2_U1Result out{};

    if (cells.head_out != nullptr && cells.rdx_in != nullptr) {
        *cells.head_out = cells.rdx_in;
    }
    ++out.mov_rsp470_rdx_count;

    // Next: vmovdqa [rsp+0x478],xmm0 @ 0x532aba — see vmovdqa_532aba_u1.
    out.ok = (out.mov_rsp470_rdx_count == 1);
    return out;
}

}  // namespace sotc
