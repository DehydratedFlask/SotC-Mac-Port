#include "init_531d60_post_base_vcall18_u5.hpp"

namespace sotc {

Init531d60PostBaseVcall18U5Result init_531d60_post_base_vcall18_u5(
    const Init531d60PostBaseVcall18U5Hooks& hooks) noexcept {
    Init531d60PostBaseVcall18U5Result out{};

    // OBS: mov rax,[rsp+0xad0]; mov rdi,r14 — stack/vtable prep NOT modelled.

    // OBS: call [rax+0x18] (callee body NOT modelled)
    if (hooks.call_vslot18 != nullptr) {
        hooks.call_vslot18();
    }
    ++out.call_vslot18_count;

    // Next: lea/mov path @ 0x532285 (toward call 0x532ed0) — NOT modelled here.
    out.ok = (out.call_vslot18_count == 1);
    return out;
}

}  // namespace sotc
