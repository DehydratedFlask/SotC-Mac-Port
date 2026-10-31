#include "init_531d60_post_base_mov_rax_rdi_mem_532b06_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRaxRdiMem_532b06_U1Result init_531d60_post_base_mov_rax_rdi_mem_532b06_u1(
    const Init531d60PostBaseMovRaxRdiMem_532b06_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRaxRdiMem_532b06_U1Result out{};
    ++out.mov_rax_rdi_mem_count;
    if (cells.cell_ptr != nullptr) {
        out.rax = *cells.cell_ptr;
    }
    // Next: call [rax+0x8] @ 0x532b09 — see continue_e.
    out.ok = (out.mov_rax_rdi_mem_count == 1);
    return out;
}

}  // namespace sotc
