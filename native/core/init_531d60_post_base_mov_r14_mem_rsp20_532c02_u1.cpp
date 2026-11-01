#include "init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1.hpp"

namespace sotc {

Init531d60PostBaseMovR14MemRsp20_532c02_U1Result init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1(
    const Init531d60PostBaseMovR14MemRsp20_532c02_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR14MemRsp20_532c02_U1Result out{};

    void* r14 = nullptr;
    if (cells.rsp20_in != nullptr) {
        r14 = const_cast<void*>(*cells.rsp20_in);
    }
    out.r14_loaded = r14;
    if (cells.r14_out != nullptr) {
        *cells.r14_out = r14;
    }
    ++out.mov_r14_mem_rsp20_532c02_count;

    // Next: mov edx, imm32 @ 0x532c07 — not modelled here.
    out.ok = (out.mov_r14_mem_rsp20_532c02_count == 1);
    return out;
}

}  // namespace sotc
