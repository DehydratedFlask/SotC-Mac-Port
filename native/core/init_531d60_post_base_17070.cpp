#include "init_531d60_post_base_17070.hpp"

namespace sotc {

Init531d60PostBase17070Result init_531d60_post_base_17070(
    const Init531d60PostBase17070Hooks& hooks) noexcept {
    Init531d60PostBase17070Result out{};

    // OBS: arg prep mov edi,0x460 @ 0x532377..0x53237c (5 B) NOT modelled.

    // OBS: call 0x17070 (body NOT modelled)
    if (hooks.call_17070 != nullptr) {
        hooks.call_17070();
    }
    ++out.call_17070_count;

    // Next: mov rbx,rax @ 0x532381 — NOT modelled here.
    out.ok = (out.call_17070_count == 1);
    return out;
}

}  // namespace sotc
