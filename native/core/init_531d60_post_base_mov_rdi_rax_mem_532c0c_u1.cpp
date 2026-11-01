#include "init_531d60_post_base_mov_rdi_rax_mem_532c0c_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRdiRaxMem_532c0c_U1Result init_531d60_post_base_mov_rdi_rax_mem_532c0c_u1(
    const Init531d60PostBaseMovRdiRaxMem_532c0c_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRdiRaxMem_532c0c_U1Result out{};
    ++out.mov_rdi_rax_mem_532c0c_count;
    if (cells.cell_ptr != nullptr) {
        out.rdi = *cells.cell_ptr;
    }
    // Next: tail @ 0x532c0f — not modelled here.
    out.ok = (out.mov_rdi_rax_mem_532c0c_count == 1);
    return out;
}

}  // namespace sotc
