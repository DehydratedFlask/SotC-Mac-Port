#include "ctor_slot_407_5d41c0.hpp"

namespace sotc {

Slot407Result init_slot407_5d41c0(Slot407Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot407Result out{};
    out.var = init_bool_var_site(objects.var, kSlot407VarName, kSlot407FnTag, concmd, 1, 1);
    if (!out.var.ok) return out;
    // 0x5d42cd..0x5d4311, original store order (vmovlps/vmovups of a zero xmm, dword 1.0 stores).
    std::uint32_t* const v = objects.vec3.data();      // rax = 0x2ff649c
    std::uint32_t* const m = objects.matrix.data();    // rcx = 0x2ff64ac
    v[0] = 0;                 // vmovlps [rax], xmm0
    v[1] = 0;
    m[0] = kSlot407One;       // dword [rcx] = 1.0
    m[1] = m[2] = m[3] = m[4] = 0;                       // xmmword [rcx+4] = 0
    m[5] = kSlot407One;       // dword [rcx+0x14] = 1.0
    m[6] = m[7] = m[8] = m[9] = 0;                       // xmmword [rcx+0x18] = 0
    m[10] = kSlot407One;      // dword [rcx+0x28] = 1.0
    v[2] = 0;                 // dword [rax+8] = 0
    m[11] = m[12] = m[13] = m[14] = 0;                   // xmmword [rcx+0x2c] = 0
    m[15] = kSlot407One;      // dword [rcx+0x3c] = 1.0
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
