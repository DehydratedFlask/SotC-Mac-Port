#include "init_531d60_post_base_f590_a2ed00_u2.hpp"

namespace sotc {

Init531d60PostBaseF590A2ed00U2Result init_531d60_post_base_f590_a2ed00_u2(
    const Init531d60PostBaseF590A2ed00U2Hooks& hooks) noexcept {
    Init531d60PostBaseF590A2ed00U2Result out{};

    // OBS: prep mov rdi,r15 @ 0x53246a..0x53246d (3 B) NOT modelled.

    // OBS: call 0xf590 (body NOT modelled)
    if (hooks.call_f590 != nullptr) {
        hooks.call_f590();
    }
    ++out.call_f590_count;

    // Next: mov rdi,[r13] @ 0x532472 — NOT modelled here.
    out.ok = (out.call_f590_count == 1);
    return out;
}

}  // namespace sotc
