#include "init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1.hpp"

namespace sotc {

Init531d60PostBaseMovR13MemRsp40_532b5b_U1Result init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1(
    const Init531d60PostBaseMovR13MemRsp40_532b5b_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR13MemRsp40_532b5b_U1Result out{};

    void* r13 = nullptr;
    if (cells.rsp40_in != nullptr) {
        r13 = const_cast<void*>(*cells.rsp40_in);
    }
    out.r13_loaded = r13;
    if (cells.r13_out != nullptr) {
        *cells.r13_out = r13;
    }
    ++out.mov_r13_mem_rsp40_count;

    // Next: mov r14,[rsp+0x48] @ 0x532b60 — peeled in E567 sibling unit.
    out.ok = (out.mov_r13_mem_rsp40_count == 1);
    return out;
}

}  // namespace sotc
