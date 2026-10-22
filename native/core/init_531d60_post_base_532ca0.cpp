#include "init_531d60_post_base_532ca0.hpp"

namespace sotc {

Init531d60PostBase532ca0Result init_531d60_post_base_532ca0(
    const Init531d60PostBase532ca0Hooks& hooks) noexcept {
    Init531d60PostBase532ca0Result out{};

    // OBS: stack prep 0x532045..0x5320ac — NOT modelled.

    // OBS: call 0x532ca0 (body NOT modelled)
    if (hooks.call_532ca0 != nullptr) {
        hooks.call_532ca0();
    }
    ++out.call_532ca0_count;

    // Next: movq / call *0x18(rax) @ 0x5320b1 — NOT modelled here.
    out.ok = (out.call_532ca0_count == 1);
    return out;
}

}  // namespace sotc
