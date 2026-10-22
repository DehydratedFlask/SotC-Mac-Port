#include "init_531d60_post_base_f590_a2ed00_u4.hpp"

namespace sotc {

Init531d60PostBaseF590A2ed00U4Result init_531d60_post_base_f590_a2ed00_u4(
    const Init531d60PostBaseF590A2ed00U4Hooks& hooks) noexcept {
    Init531d60PostBaseF590A2ed00U4Result out{};

    // OBS: prep mov rdi,r15 @ 0x5324e0..0x5324e3 (3 B) NOT modelled.

    // OBS: call 0xf590 (body NOT modelled)
    if (hooks.call_f590 != nullptr) {
        hooks.call_f590();
    }
    ++out.call_f590_count;

    // Next: mov rdi,[r13] @ 0x5324e8 — NOT modelled here.
    out.ok = (out.call_f590_count == 1);
    return out;
}

}  // namespace sotc
