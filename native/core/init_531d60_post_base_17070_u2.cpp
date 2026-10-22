#include "init_531d60_post_base_17070_u2.hpp"

namespace sotc {

Init531d60PostBase17070U2Result init_531d60_post_base_17070_u2(
    const Init531d60PostBase17070U2Hooks& hooks) noexcept {
    Init531d60PostBase17070U2Result out{};

    // OBS: arg prep mov edi,0x490 @ 0x5323a9..0x5323ae (5 B) NOT modelled.

    // OBS: call 0x17070 (body NOT modelled)
    if (hooks.call_17070 != nullptr) {
        hooks.call_17070();
    }
    ++out.call_17070_count;

    // Next: mov rbx,rax @ 0x5323b3 — NOT modelled here.
    out.ok = (out.call_17070_count == 1);
    return out;
}

}  // namespace sotc
