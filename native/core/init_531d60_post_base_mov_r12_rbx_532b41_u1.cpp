#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"

#include "registry_init_531d60.hpp"

namespace sotc {

Init531d60PostBaseMovR12Rbx_532b41_U1Result init_531d60_post_base_mov_r12_rbx_532b41_u1(
    const Init531d60PostBaseMovR12Rbx_532b41_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR12Rbx_532b41_U1Result out{};

    void* loaded = (cells.cell_word != nullptr) ? *cells.cell_word : registry_cell_2efd160().load();
    out.r12 = loaded;
    if (cells.r12_out != nullptr) {
        *cells.r12_out = loaded;
    }
    ++out.mov_r12_rbx_count;

    // Next: cmp r12,r14 @ 0x532b44 — peeled as registry_body unit E561.
    out.ok = (out.mov_r12_rbx_count == 1);
    return out;
}

}  // namespace sotc
