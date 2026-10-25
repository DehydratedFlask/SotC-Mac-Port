#include "init_531d60_post_base_532ca0_u4.hpp"

namespace sotc {

Init531d60PostBase532ca0U4Result init_531d60_post_base_532ca0_u4(
    const Init531d60PostBase532ca0U4Hooks& hooks) noexcept {
    Init531d60PostBase532ca0U4Result out{};

    // OBS: stack prep 0x5321a2..0x5321ff — NOT modelled.

    // OBS: call 0x532ca0 (body NOT modelled)
    if (hooks.call_532ca0 != nullptr) {
        hooks.call_532ca0();
    }
    ++out.call_532ca0_count;

    // Next: movq / call *0x18(rax) @ 0x532204 — NOT modelled here.
    out.ok = (out.call_532ca0_count == 1);
    return out;
}

}  // namespace sotc
