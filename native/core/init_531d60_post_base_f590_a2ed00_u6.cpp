#include "init_531d60_post_base_f590_a2ed00_u6.hpp"

namespace sotc {

Init531d60PostBaseF590A2ed00U6Result init_531d60_post_base_f590_a2ed00_u6(
    const Init531d60PostBaseF590A2ed00U6Hooks& hooks) noexcept {
    Init531d60PostBaseF590A2ed00U6Result out{};

    // OBS: prep mov rdi,r15 @ 0x53255d..0x532560 (3 B) NOT modelled.

    // OBS: call 0xf590 on the measured 32 B stack temp (rsp+0x158, `+0x10..0x1f` zeroed): no
    // control block, so the lifted release is a no-op unless the host overrides it.
    SharedHolderF590 holder{};
    if (hooks.call_f590 != nullptr) {
        out.release = hooks.call_f590(holder);
    } else {
        out.release = shared_release_f590(holder, hooks.release_hooks);
    }
    ++out.call_f590_count;

    // Next: mov rdi,[r13] / stack prep / call 0xa2ed00 @ 0x532565 — NOT modelled here.
    out.ok = (out.call_f590_count == 1);
    return out;
}

}  // namespace sotc
