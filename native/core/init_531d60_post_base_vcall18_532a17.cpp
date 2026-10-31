#include "init_531d60_post_base_vcall18_532a17.hpp"

namespace sotc {

Init531d60PostBaseVcall18532a17Result init_531d60_post_base_vcall18_532a17(
    const Init531d60PostBaseVcall18532a17Hooks& hooks) noexcept {
    Init531d60PostBaseVcall18532a17Result out{};

    // OBS: mov rax,[rsp+0x560]; mov rdi,r14 — stack/vtable prep NOT modelled.

    // OBS: call [rax+0x18] → 0x538200 (bare ret; elided when hook null).
    if (hooks.call_vslot18 != nullptr) {
        hooks.call_vslot18();
    }
    ++out.call_vslot18_count;

    // Next: mov rdi,[r13] @ 0x532a1a — NOT modelled here.
    out.ok = (out.call_vslot18_count == 1);
    return out;
}

}  // namespace sotc
