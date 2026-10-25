#include "init_531d60_post_base_5b3d00.hpp"

namespace sotc {

Init531d60PostBase5b3d00Result init_531d60_post_base_5b3d00(
    const Init531d60PostBase5b3d00Hooks& hooks) noexcept {
    Init531d60PostBase5b3d00Result out{};

    // OBS: call 0x5b3d00 (target is ret-nop)
    if (hooks.call_5b3d00 != nullptr) {
        hooks.call_5b3d00();
    }
    ++out.call_5b3d00_count;

    // Next: call 0x5b3cf0 @ 0x531e12 — NOT modelled here.
    out.ok = (out.call_5b3d00_count == 1);
    return out;
}

}  // namespace sotc
