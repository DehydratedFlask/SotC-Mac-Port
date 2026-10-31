#include "init_531d60_post_base_mov_r14_mem_rsp48_532b60_u1.hpp"

namespace sotc {

Init531d60PostBaseMovR14MemRsp48_532b60_U1Result init_531d60_post_base_mov_r14_mem_rsp48_532b60_u1(
    const Init531d60PostBaseMovR14MemRsp48_532b60_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR14MemRsp48_532b60_U1Result out{};

    void* r14 = nullptr;
    if (cells.rsp48_in != nullptr) {
        r14 = const_cast<void*>(*cells.rsp48_in);
    }
    out.r14_loaded = r14;
    if (cells.r14_out != nullptr) {
        *cells.r14_out = r14;
    }
    ++out.mov_r14_mem_rsp48_count;

    // Next: cmp r13,r14 @ 0x532b65 — not modelled in this unit.
    out.ok = (out.mov_r14_mem_rsp48_count == 1);
    return out;
}

}  // namespace sotc
