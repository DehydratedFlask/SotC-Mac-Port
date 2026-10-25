#include "init_531d60_post_base_5c64c0.hpp"

namespace sotc {

Init531d60PostBase5c64c0Result init_531d60_post_base_5c64c0(
    const Init531d60PostBase5c64c0Hooks& hooks) noexcept {
    Init531d60PostBase5c64c0Result out{};

    // OBS: call 0x5c64c0
    if (hooks.call_5c64c0 != nullptr) {
        hooks.call_5c64c0();
    }
    ++out.call_5c64c0_count;

    // Next: call 0x1290 @ 0x531e03 — NOT modelled here.
    out.ok = (out.call_5c64c0_count == 1);
    return out;
}

}  // namespace sotc
