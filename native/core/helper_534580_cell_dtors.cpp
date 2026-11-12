#include "helper_534580_cell_dtors.hpp"

namespace sotc {

bool cell_dtor_534580(void* const* cell, const CellDtorHooks& hooks) noexcept {
    void* const pointer = *cell;                          // mov rdi,[rdi] ; test rdi,rdi ; je ret
    if (pointer == nullptr) return true;
    if (hooks.free_type2 == nullptr) return false;
    hooks.free_type2(pointer);                            // jmp 0x170c0 (tail call; the cell is not cleared)
    return true;
}

bool cell_dtor_13a0d0(void** cell, const CellDtorHooks& hooks) noexcept {
    void* const pointer = *cell;                          // mov rdi,[rbx] ; test rdi,rdi ; je ret
    if (pointer == nullptr) return true;
    if (hooks.virtual_delete == nullptr) return false;
    hooks.virtual_delete(pointer);                        // mov rax,[rdi] ; call [rax+8]
    *cell = nullptr;                                      // mov qword [rbx],0
    return true;
}

}  // namespace sotc
