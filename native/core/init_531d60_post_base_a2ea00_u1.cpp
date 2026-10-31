#include "init_531d60_post_base_a2ea00_u1.hpp"

namespace sotc {

Init531d60PostBaseA2ea00U1Result init_531d60_post_base_a2ea00_u1(
    const Init531d60PostBaseA2ea00U1Hooks& hooks) noexcept {
    Init531d60PostBaseA2ea00U1Result out{};

    // OBS: prep 0x532728..0x532731 (9 B) NOT modelled.

    if (hooks.call_a2ea00 != nullptr) {
        out.eax_return = hooks.call_a2ea00(const_cast<void*>(hooks.incoming_rbx));
    }
    ++out.call_a2ea00_count;

    // Next: mov rdi,rbx @ 0x532736 — NOT modelled here.
    out.ok = (out.call_a2ea00_count == 1);
    return out;
}

}  // namespace sotc
