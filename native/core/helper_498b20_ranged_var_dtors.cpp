#include "helper_498b20_ranged_var_dtors.hpp"

#include <cstring>

namespace sotc {

const RangedVarDtorRow* find_ranged_var_dtor(std::uint64_t rva) noexcept {
    for (const RangedVarDtorRow& row : kRangedVarDtors) if (row.rva == rva) return &row;
    return nullptr;
}

bool ranged_var_dtor_run(std::uint64_t rva, void* object, const RangedVarDtorHooks& hooks) noexcept {
    const RangedVarDtorRow* row = find_ranged_var_dtor(rva);
    if (row == nullptr || hooks.functor_destroy == nullptr || hooks.base_dtor == nullptr) return false;
    auto* bytes = static_cast<std::uint8_t*>(object);
    std::memcpy(bytes, &row->vptr, sizeof row->vptr);                       // mov [rbx], vptr
    std::uint64_t fn_vtbl;
    std::memcpy(&fn_vtbl, bytes + kRangedVarDtorFnVptrOffset, sizeof fn_vtbl);   // mov rax,[rbx+0xc0]
    hooks.functor_destroy(fn_vtbl, bytes + kRangedVarDtorFnOffset);         // lea rdi,[rbx+0xa0] ; call [rax+0x18]
    hooks.base_dtor(object);                                                // mov rdi,rbx ; jmp 0x20b50
    return true;
}

}  // namespace sotc
