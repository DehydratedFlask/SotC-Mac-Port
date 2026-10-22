#include "init_531d60_post_base_56db70.hpp"

namespace sotc {

Init531d60PostBase56db70Result init_531d60_post_base_56db70(
    const Init531d60PostBase56db70Hooks& hooks) noexcept {
    Init531d60PostBase56db70Result out{};

    // OBS: call 0x56db70 (target is ret-nop)
    if (hooks.call_56db70 != nullptr) {
        hooks.call_56db70();
    }
    ++out.call_56db70_count;

    // Next: call 0x5b3d00 @ 0x531e0d — NOT modelled here.
    out.ok = (out.call_56db70_count == 1);
    return out;
}

}  // namespace sotc
