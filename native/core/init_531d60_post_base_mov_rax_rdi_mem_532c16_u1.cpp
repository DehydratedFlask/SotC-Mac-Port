#include "init_531d60_post_base_mov_rax_rdi_mem_532c16_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRaxRdiMem_532c16_U1Result init_531d60_post_base_mov_rax_rdi_mem_532c16_u1(
    const Init531d60PostBaseMovRaxRdiMem_532c16_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRaxRdiMem_532c16_U1Result out{};

    if (cells.cell_ptr != nullptr) {
        out.rax = const_cast<void*>(*cells.cell_ptr);
    }
    if (cells.rax_out != nullptr) {
        *cells.rax_out = out.rax;
    }
    ++out.mov_rax_rdi_mem_532c16_count;

    // Next: E615 call [rax+0x70] @ 0x532c19 — separate unit.
    out.ok = (out.mov_rax_rdi_mem_532c16_count == 1);
    return out;
}

}  // namespace sotc
