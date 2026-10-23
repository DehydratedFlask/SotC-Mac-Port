#include "init_531d60_post_base_f590_a2ed00_u3.hpp"

namespace sotc {

Init531d60PostBaseF590A2ed00U3Result init_531d60_post_base_f590_a2ed00_u3(
    const Init531d60PostBaseF590A2ed00U3Hooks& hooks) noexcept {
    Init531d60PostBaseF590A2ed00U3Result out{};

    // OBS: prep mov rdi,rbx @ 0x5324d8..0x5324db (3 B) NOT modelled.

    // OBS: call 0xf590 on the measured 32 B stack temp (rsp+0x178, `+0x10..0x1f` zeroed): no
    // control block, so the lifted release is a no-op unless the host overrides it.
    SharedHolderF590 holder{};
    if (hooks.call_f590 != nullptr) {
        out.release = hooks.call_f590(holder);
    } else {
        out.release = shared_release_f590(holder, hooks.release_hooks);
    }
    ++out.call_f590_count;

    // Next: mov rdi,r15 / call 0xf590 @ 0x5324e0 — NOT modelled here.
    out.ok = (out.call_f590_count == 1);
    return out;
}

}  // namespace sotc
