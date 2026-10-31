#include "init_531d60_post_base_vmovdqa_rsp478_xmm0_532aba_u1.hpp"

#include <cstring>

namespace sotc {

Init531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1Result
init_531d60_post_base_vmovdqa_rsp478_xmm0_532aba_u1(
    const Init531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1Cells& cells) noexcept {
    Init531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1Result out{};

    alignas(16) std::uint8_t zero_lane[16]{};
    const std::uint8_t* src = cells.xmm0_in != nullptr ? cells.xmm0_in : zero_lane;
    if (cells.dest_out != nullptr) {
        std::memcpy(cells.dest_out, src, kInit531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1LaneBytes);
    }
    ++out.vmovdqa_count;

    // Next: mov [rsp+0x488],rax @ 0x532ac3 — see mov_rsp488_rax_532ac3_u1.
    out.ok = (out.vmovdqa_count == 1);
    return out;
}

}  // namespace sotc
