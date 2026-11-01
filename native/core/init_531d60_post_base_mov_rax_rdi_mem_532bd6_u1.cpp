#include "init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRaxRdiMem_532bd6_U1Result init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1(
    const Init531d60PostBaseMovRaxRdiMem_532bd6_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRaxRdiMem_532bd6_U1Result out{};
    ++out.mov_rax_rdi_mem_532bd6_count;
    if (cells.cell_ptr != nullptr) {
        out.rax = *cells.cell_ptr;
    }
    // Next: tail @ 0x532bd9 — not modelled here.
    out.ok = (out.mov_rax_rdi_mem_532bd6_count == 1);
    return out;
}

}  // namespace sotc
