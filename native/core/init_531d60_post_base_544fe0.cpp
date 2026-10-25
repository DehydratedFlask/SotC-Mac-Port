#include "init_531d60_post_base_544fe0.hpp"

namespace sotc {

Init531d60PostBase544fe0Result init_531d60_post_base_544fe0(
    const Init531d60PostBase544fe0Hooks& hooks) noexcept {
    Init531d60PostBase544fe0Result out{};

    // OBS: prep mov rbx,rax; mov rdi,rbx @ 0x532381..0x532387 (6 B) NOT modelled.

    // OBS: call 0x544fe0 (body NOT modelled)
    if (hooks.call_544fe0 != nullptr) {
        hooks.call_544fe0();
    }
    ++out.call_544fe0_count;

    // Next: lea r14 @ 0x53238c — NOT modelled here.
    out.ok = (out.call_544fe0_count == 1);
    return out;
}

}  // namespace sotc
