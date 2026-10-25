#include "init_531d60_post_base_1290.hpp"

namespace sotc {

Init531d60PostBase1290Result init_531d60_post_base_1290(
    const Init531d60PostBase1290Hooks& hooks) noexcept {
    Init531d60PostBase1290Result out{};

    // OBS: call 0x1290
    if (hooks.call_1290 != nullptr) {
        hooks.call_1290();
    }
    ++out.call_1290_count;

    // Next: call 0x56db70 @ 0x531e08 — NOT modelled here.
    out.ok = (out.call_1290_count == 1);
    return out;
}

}  // namespace sotc
