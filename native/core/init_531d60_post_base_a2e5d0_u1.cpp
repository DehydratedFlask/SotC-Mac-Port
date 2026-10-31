#include "init_531d60_post_base_a2e5d0_u1.hpp"

namespace sotc {

Init531d60PostBaseA2e5d0U1Result init_531d60_post_base_a2e5d0_u1(
    const Init531d60PostBaseA2e5d0U1Hooks& hooks) noexcept {
    Init531d60PostBaseA2e5d0U1Result out{};

    // OBS: prep 0x532736..0x53273b (5 B: mov rdi,rbx / mov esi,eax) NOT modelled.

    if (hooks.call_a2e5d0 != nullptr) {
        hooks.call_a2e5d0(const_cast<void*>(hooks.incoming_rbx), hooks.incoming_esi);
    }
    ++out.call_a2e5d0_count;

    // Next: lea rax,[0x32f79a8] @ 0x532740 — NOT modelled here.
    out.ok = (out.call_a2e5d0_count == 1);
    return out;
}

}  // namespace sotc
