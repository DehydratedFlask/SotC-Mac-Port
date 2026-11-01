#include "init_531d60_post_base_mov_rdi_mem_r12_532bdb_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRdiMemR12_532bdb_U1Result init_531d60_post_base_mov_rdi_mem_r12_532bdb_u1(
    const Init531d60PostBaseMovRdiMemR12_532bdb_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRdiMemR12_532bdb_U1Result out{};

    if (cells.cell_ptr != nullptr) {
        out.rdi = const_cast<void*>(*cells.cell_ptr);
    }
    if (cells.rdi_out != nullptr) {
        *cells.rdi_out = out.rdi;
    }
    ++out.mov_rdi_mem_r12_532bdb_count;

    // Next: E613 mov r13,[0x15fe518] @ 0x532bdf — separate unit.
    out.ok = (out.mov_rdi_mem_r12_532bdb_count == 1);
    return out;
}

}  // namespace sotc
