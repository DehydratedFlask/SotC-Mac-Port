#include "init_531d60_post_base_vcall18_532a72_u1.hpp"

namespace sotc {

Init531d60PostBaseVcall18_532a72_U1Result init_531d60_post_base_vcall18_532a72_u1(
    const Init531d60PostBaseVcall18_532a72_U1Hooks& hooks) noexcept {
    Init531d60PostBaseVcall18_532a72_U1Result out{};

    // OBS: mov rax,[rsp+0x500]; mov rdi,r14 — stack/vtable prep NOT modelled.

    if (hooks.call_vslot18 != nullptr) {
        hooks.call_vslot18();
    }
    ++out.call_vslot18_count;

    // Next: vmovq @ 0x532a75 — NOT modelled here.
    out.ok = (out.call_vslot18_count == 1);
    return out;
}

}  // namespace sotc
