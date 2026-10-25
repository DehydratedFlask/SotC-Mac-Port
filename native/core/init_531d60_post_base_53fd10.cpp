#include "init_531d60_post_base_53fd10.hpp"

namespace sotc {

Init531d60PostBase53fd10Result init_531d60_post_base_53fd10(
    const Init531d60PostBase53fd10Hooks& hooks) noexcept {
    Init531d60PostBase53fd10Result out{};

    // OBS: prep mov rbx,rax; mov rdi,rbx @ 0x5323b3..0x5323b9 (6 B) NOT modelled.

    // OBS: call 0x53fd10 (body NOT modelled)
    if (hooks.call_53fd10 != nullptr) {
        hooks.call_53fd10();
    }
    ++out.call_53fd10_count;

    // Next: lea r14 @ 0x5323be — NOT modelled here.
    out.ok = (out.call_53fd10_count == 1);
    return out;
}

}  // namespace sotc
