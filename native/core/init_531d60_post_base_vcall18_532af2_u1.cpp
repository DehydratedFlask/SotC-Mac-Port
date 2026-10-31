#include "init_531d60_post_base_vcall18_532af2_u1.hpp"

namespace sotc {

Init531d60PostBaseVcall18_532af2_U1Result init_531d60_post_base_vcall18_532af2_u1(
    const Init531d60PostBaseVcall18_532af2_U1Hooks& hooks) noexcept {
    Init531d60PostBaseVcall18_532af2_U1Result out{};

    // OBS: mov rdi,rbx — prep NOT modelled.

    if (hooks.call_vslot18 != nullptr) {
        hooks.call_vslot18();
    }
    ++out.call_vslot18_count;

    // Next: lea rax,[0x2eff450] @ 0x532af5 — NOT modelled here.
    out.ok = (out.call_vslot18_count == 1);
    return out;
}

}  // namespace sotc
