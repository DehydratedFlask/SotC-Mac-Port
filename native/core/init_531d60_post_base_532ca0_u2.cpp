#include "init_531d60_post_base_532ca0_u2.hpp"

namespace sotc {

Init531d60PostBase532ca0U2Result init_531d60_post_base_532ca0_u2(
    const Init531d60PostBase532ca0U2Hooks& hooks) noexcept {
    Init531d60PostBase532ca0U2Result out{};

    // OBS: stack prep 0x5320bf..0x53211f — NOT modelled.

    // OBS: call 0x532ca0 (body NOT modelled)
    if (hooks.call_532ca0 != nullptr) {
        hooks.call_532ca0();
    }
    ++out.call_532ca0_count;

    // Next: movq / call *0x18(rax) @ 0x532124 — NOT modelled here.
    out.ok = (out.call_532ca0_count == 1);
    return out;
}

}  // namespace sotc
