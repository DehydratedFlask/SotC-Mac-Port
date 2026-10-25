#include "init_531d60_post_base_vcall18.hpp"

namespace sotc {

Init531d60PostBaseVcall18Result init_531d60_post_base_vcall18(
    const Init531d60PostBaseVcall18Hooks& hooks) noexcept {
    Init531d60PostBaseVcall18Result out{};

    // OBS: mov rax,[rsp+0xc10]; mov rdi,r14 — stack/vtable prep NOT modelled.

    // OBS: call [rax+0x18] (callee body NOT modelled)
    if (hooks.call_vslot18 != nullptr) {
        hooks.call_vslot18();
    }
    ++out.call_vslot18_count;

    // Next: mov rdi,[rbx] / second unit @ 0x5320bf — NOT modelled here.
    out.ok = (out.call_vslot18_count == 1);
    return out;
}

}  // namespace sotc
