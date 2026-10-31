#include "init_531d60_post_base_movq_rsp4a0_0_532a93_u1.hpp"

namespace sotc {

Init531d60PostBaseMovqRsp4a0_0_532a93_U1Result init_531d60_post_base_movq_rsp4a0_0_532a93_u1(
    const Init531d60PostBaseMovqRsp4a0_0_532a93_U1Cells& cells) noexcept {
    Init531d60PostBaseMovqRsp4a0_0_532a93_U1Result out{};

    if (cells.field_30_out != nullptr) {
        *cells.field_30_out = 0;
    }
    ++out.movq_zero_count;

    // Next: mov rdi,[r13] @ 0x532a9f — NOT modelled here.
    out.ok = (out.movq_zero_count == 1);
    return out;
}

}  // namespace sotc
