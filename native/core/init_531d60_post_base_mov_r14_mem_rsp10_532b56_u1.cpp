#include "init_531d60_post_base_mov_r14_mem_rsp10_532b56_u1.hpp"

namespace sotc {

Init531d60PostBaseMovR14MemRsp10_532b56_U1Result init_531d60_post_base_mov_r14_mem_rsp10_532b56_u1(
    const Init531d60PostBaseMovR14MemRsp10_532b56_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR14MemRsp10_532b56_U1Result out{};

    void* r14 = const_cast<void*>(cells.r14_in);
    out.r14_stored = r14;
    if (cells.rsp10_out != nullptr) {
        *cells.rsp10_out = r14;
    }
    ++out.mov_r14_mem_rsp10_count;

    // Next: mov r13,[rsp+0x40] @ 0x532b5b — not modelled in this unit.
    out.ok = (out.mov_r14_mem_rsp10_count == 1);
    return out;
}

}  // namespace sotc
