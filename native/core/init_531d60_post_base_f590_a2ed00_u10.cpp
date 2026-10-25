#include "init_531d60_post_base_f590_a2ed00_u10.hpp"

namespace sotc {

Init531d60PostBaseF590A2ed00U10Result init_531d60_post_base_f590_a2ed00_u10(
    const Init531d60PostBaseF590A2ed00U10Hooks& hooks) noexcept {
    Init531d60PostBaseF590A2ed00U10Result out{};

    // OBS: prep mov rdi,r15 @ 0x53264c..0x53264f (3 B) NOT modelled.

    // OBS: call 0xf590 on the measured 32 B stack temp (rsp+0xd8, `+0x10..0x1f` zeroed): no
    // control block, so the lifted release is a no-op unless the host overrides it.
    SharedHolderF590 holder{};
    if (hooks.call_f590 != nullptr) {
        out.release = hooks.call_f590(holder);
    } else {
        out.release = shared_release_f590(holder, hooks.release_hooks);
    }
    ++out.call_f590_count;

    // Next: mov rdi,[r13] / stack prep / call 0xa2ed00 @ 0x532654 — NOT modelled here.
    out.ok = (out.call_f590_count == 1);
    return out;
}

}  // namespace sotc
