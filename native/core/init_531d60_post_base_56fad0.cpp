#include "init_531d60_post_base_56fad0.hpp"

namespace sotc {

Init531d60PostBase56fad0Result init_531d60_post_base_56fad0(
    const Init531d60PostBase56fad0Hooks& hooks) noexcept {
    Init531d60PostBase56fad0Result out{};

    // OBS: call 0x56fad0 (target is ret-nop)
    if (hooks.call_56fad0 != nullptr) {
        hooks.call_56fad0();
    }
    ++out.call_56fad0_count;

    // Next: call 0xf90310 @ 0x531e46 — NOT modelled here.
    out.ok = (out.call_56fad0_count == 1);
    return out;
}

}  // namespace sotc
