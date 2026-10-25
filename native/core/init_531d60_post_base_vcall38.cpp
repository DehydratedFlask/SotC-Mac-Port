#include "init_531d60_post_base_vcall38.hpp"

namespace sotc {

Init531d60PostBaseVcall38Result init_531d60_post_base_vcall38(
    const Init531d60PostBaseVcall38Hooks& hooks) noexcept {
    Init531d60PostBaseVcall38Result out{};

    // OBS: mov rax,[r12]; mov rdi,r12 — r12/vtable producers NOT modelled.

    // OBS: call [rax+0x38] (callee body NOT modelled)
    if (hooks.call_vslot38 != nullptr) {
        hooks.call_vslot38();
    }
    ++out.call_vslot38_count;

    // Next: lea r13 @ 0x5323e5 — NOT modelled here.
    out.ok = (out.call_vslot38_count == 1);
    return out;
}

}  // namespace sotc
