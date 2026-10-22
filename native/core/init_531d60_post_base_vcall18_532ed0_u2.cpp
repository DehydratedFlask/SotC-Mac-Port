#include "init_531d60_post_base_vcall18_532ed0_u2.hpp"

namespace sotc {

Init531d60PostBaseVcall18532ed0U2Result init_531d60_post_base_vcall18_532ed0_u2(
    const Init531d60PostBaseVcall18532ed0U2Hooks& hooks) noexcept {
    Init531d60PostBaseVcall18532ed0U2Result out{};

    // OBS: mov rax,[rsp+0xa30]; mov rdi,r14 — stack/vtable prep NOT modelled.

    // OBS: call [rax+0x18] (callee body NOT modelled)
    if (hooks.call_vslot18 != nullptr) {
        hooks.call_vslot18();
    }
    ++out.call_vslot18_count;

    // Next: mov edi,0x460 / call 0x17070 @ 0x532377 — NOT modelled here.
    out.ok = (out.call_vslot18_count == 1);
    return out;
}

}  // namespace sotc
