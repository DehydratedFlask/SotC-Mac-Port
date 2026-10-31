#include "init_531d60_post_base_mov_rsp4b0_rax_532ae7_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRsp4b0_Rax_532ae7_U1Result init_531d60_post_base_mov_rsp4b0_rax_532ae7_u1(
    const Init531d60PostBaseMovRsp4b0_Rax_532ae7_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRsp4b0_Rax_532ae7_U1Result out{};

    void* val = nullptr;
    if (cells.vtable_stack_in != nullptr) {
        val = const_cast<void*>(*cells.vtable_stack_in);
    }
    if (val == nullptr) {
        val = reinterpret_cast<void*>(static_cast<std::uintptr_t>(
            kInit531d60PostBaseMovRsp4b0_Rax_532ae7_U1DefaultVtableRva));
    }
    if (cells.rax_out != nullptr) {
        *cells.rax_out = val;
    }
    ++out.mov_rsp4b0_rax_count;

    // Next: mov rdi,rbx @ 0x532aef — see continue_e.
    out.ok = (out.mov_rsp4b0_rax_count == 1);
    return out;
}

}  // namespace sotc
