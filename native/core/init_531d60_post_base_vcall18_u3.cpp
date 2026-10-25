#include "init_531d60_post_base_vcall18_u3.hpp"

namespace sotc {

Init531d60PostBaseVcall18U3Result init_531d60_post_base_vcall18_u3(
    const Init531d60PostBaseVcall18U3Hooks& hooks) noexcept {
    Init531d60PostBaseVcall18U3Result out{};

    // OBS: mov rax,[rsp+0xb70]; mov rdi,r14 — stack/vtable prep NOT modelled.

    // OBS: call [rax+0x18] (callee body NOT modelled)
    if (hooks.call_vslot18 != nullptr) {
        hooks.call_vslot18();
    }
    ++out.call_vslot18_count;

    // Next: mov rdi,[rbx] / next unit @ 0x5321a2 — NOT modelled here.
    out.ok = (out.call_vslot18_count == 1);
    return out;
}

}  // namespace sotc
