#include "init_531d60_post_base_f590_a2ed00_u6.hpp"

namespace sotc {

Init531d60PostBaseF590A2ed00U6Result init_531d60_post_base_f590_a2ed00_u6(
    const Init531d60PostBaseF590A2ed00U6Hooks& hooks) noexcept {
    Init531d60PostBaseF590A2ed00U6Result out{};

    // OBS: prep mov rdi,r15 @ 0x53255d..0x532560 (3 B) NOT modelled.

    // OBS: call 0xf590 (body NOT modelled)
    if (hooks.call_f590 != nullptr) {
        hooks.call_f590();
    }
    ++out.call_f590_count;

    // Next: mov rdi,[r13] / stack prep / call 0xa2ed00 @ 0x532565 — NOT modelled here.
    out.ok = (out.call_f590_count == 1);
    return out;
}

}  // namespace sotc
