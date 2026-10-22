#include "init_531d60_post_base_vcall18_u2.hpp"

namespace sotc {

Init531d60PostBaseVcall18U2Result init_531d60_post_base_vcall18_u2(
    const Init531d60PostBaseVcall18U2Hooks& hooks) noexcept {
    Init531d60PostBaseVcall18U2Result out{};

    // OBS: mov rax,[rsp+0xbc0]; mov rdi,r14 — stack/vtable prep NOT modelled.

    // OBS: call [rax+0x18] (callee body NOT modelled)
    if (hooks.call_vslot18 != nullptr) {
        hooks.call_vslot18();
    }
    ++out.call_vslot18_count;

    // Next: mov rdi,[rbx] / next unit @ 0x532132 — NOT modelled here.
    out.ok = (out.call_vslot18_count == 1);
    return out;
}

}  // namespace sotc
