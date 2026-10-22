#include "init_531d60_post_base_532ca0_u3.hpp"

namespace sotc {

Init531d60PostBase532ca0U3Result init_531d60_post_base_532ca0_u3(
    const Init531d60PostBase532ca0U3Hooks& hooks) noexcept {
    Init531d60PostBase532ca0U3Result out{};

    // OBS: stack prep 0x532132..0x53218f — NOT modelled.

    // OBS: call 0x532ca0 (body NOT modelled)
    if (hooks.call_532ca0 != nullptr) {
        hooks.call_532ca0();
    }
    ++out.call_532ca0_count;

    // Next: movq / call *0x18(rax) @ 0x532194 — NOT modelled here.
    out.ok = (out.call_532ca0_count == 1);
    return out;
}

}  // namespace sotc
