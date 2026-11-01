#include "init_531d60_post_base_mov_r14_mem_rsp10_532bba_u1.hpp"

namespace sotc {

Init531d60PostBaseMovR14MemRsp10_532bba_U1Result init_531d60_post_base_mov_r14_mem_rsp10_532bba_u1(
    const Init531d60PostBaseMovR14MemRsp10_532bba_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR14MemRsp10_532bba_U1Result out{};

    void* r14 = nullptr;
    if (cells.rsp10_in != nullptr) {
        r14 = *cells.rsp10_in;
    }
    out.r14 = r14;
    if (cells.r14_out != nullptr) {
        *cells.r14_out = r14;
    }
    ++out.mov_r14_mem_rsp10_532bba_count;

    out.ok = (out.mov_r14_mem_rsp10_532bba_count == 1);
    return out;
}

}  // namespace sotc
