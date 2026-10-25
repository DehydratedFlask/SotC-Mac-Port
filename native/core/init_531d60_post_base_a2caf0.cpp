#include "init_531d60_post_base_a2caf0.hpp"

namespace sotc {

Init531d60PostBaseA2caf0Result init_531d60_post_base_a2caf0(
    const Init531d60PostBaseA2caf0Hooks& hooks) noexcept {
    Init531d60PostBaseA2caf0Result out{};

    // OBS: prep mov rdi,[r13] @ 0x5323ec..0x5323f0 (4 B) NOT modelled.

    // OBS: call 0xa2caf0 (body NOT modelled)
    if (hooks.call_a2caf0 != nullptr) {
        hooks.call_a2caf0();
    }
    ++out.call_a2caf0_count;

    // Next: mov rdi,[r13] @ 0x5323f5 — NOT modelled here.
    out.ok = (out.call_a2caf0_count == 1);
    return out;
}

}  // namespace sotc
