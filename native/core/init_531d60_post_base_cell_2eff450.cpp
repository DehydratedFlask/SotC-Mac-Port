#include "init_531d60_post_base_cell_2eff450.hpp"

namespace sotc {

Init531d60PostBaseCell2eff450Result init_531d60_post_base_cell_2eff450(
    const Init531d60PostBaseCell2eff450Cells& cells,
    const Init531d60PostBaseCell2eff450Hooks& hooks) noexcept {
    Init531d60PostBaseCell2eff450Result out{};

    if (cells.neu == nullptr || cells.cell == nullptr) {
        return out;
    }

    // OBS: lea r14,[0x2eff450]; mov rdi,[r14]
    void* prev = *cells.cell;
    // OBS: cmp rdi,rbx; jz skip (@ 0x5323a9)
    if (prev == cells.neu) {
        out.reused_same = true;
        out.ok = true;
        return out;
    }
    // OBS: test rdi,rdi; jz store; else mov rax,[rdi]; call [rax+0x70]
    if (prev != nullptr) {
        if (hooks.dtor_vslot70 != nullptr) {
            hooks.dtor_vslot70(prev);
        }
        ++out.dtor_vslot70_count;
        out.destroyed_prev = true;
    }
    // OBS: mov [r14], rbx
    *cells.cell = cells.neu;
    out.installed = true;
    out.ok = true;
    return out;
}

}  // namespace sotc
