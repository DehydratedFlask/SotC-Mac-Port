#include "init_531d60_post_base_532ca0_u5.hpp"

namespace sotc {

Init531d60PostBase532ca0U5Result init_531d60_post_base_532ca0_u5(
    const Init531d60PostBase532ca0U5Hooks& hooks) noexcept {
    Init531d60PostBase532ca0U5Result out{};

    // OBS: stack prep 0x532212..0x532272 (96 B) NOT modelled — call-site only.

    // OBS: call 0x532ca0 (body NOT modelled)
    if (hooks.call_532ca0 != nullptr) {
        hooks.call_532ca0();
    }
    ++out.call_532ca0_count;

    // Next: movq/virtual-call path @ 0x532277 — NOT modelled here.
    out.ok = (out.call_532ca0_count == 1);
    return out;
}

}  // namespace sotc
