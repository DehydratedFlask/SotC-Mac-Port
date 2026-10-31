#include "init_531d60_post_base_mov_rdi_rax_mem_532b03_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRdiRaxMem_532b03_U1Result init_531d60_post_base_mov_rdi_rax_mem_532b03_u1(
    const Init531d60PostBaseMovRdiRaxMem_532b03_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRdiRaxMem_532b03_U1Result out{};
    ++out.mov_rdi_rax_mem_count;
    if (cells.cell_ptr != nullptr) {
        out.rdi = *cells.cell_ptr;
    }
    // Next: mov rax,[rdi] @ 0x532b06 — see mov_rax_rdi_mem_532b06_u1.
    out.ok = (out.mov_rdi_rax_mem_count == 1);
    return out;
}

}  // namespace sotc
