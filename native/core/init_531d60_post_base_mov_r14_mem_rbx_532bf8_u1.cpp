#include "init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.hpp"

#include "registry_init_531d60.hpp"

namespace sotc {

Init531d60PostBaseMovR14MemRbx_532bf8_U1Result init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1(
    const Init531d60PostBaseMovR14MemRbx_532bf8_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR14MemRbx_532bf8_U1Result out{};

    void* const r14 = cells.r14_in;
    if (cells.cell_word != nullptr) {
        *cells.cell_word = r14;
    } else {
        registry_cell_2efd160().store(static_cast<RegistryObject2efd160*>(r14));
    }
    out.stored = r14;
    ++out.mov_r14_mem_rbx_532bf8_count;

    // Next: lea rax,[rip+disp] @ 0x532bfb — not modelled here.
    out.ok = (out.mov_r14_mem_rbx_532bf8_count == 1);
    return out;
}

}  // namespace sotc
