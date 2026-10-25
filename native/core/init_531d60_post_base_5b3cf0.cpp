#include "init_531d60_post_base_5b3cf0.hpp"

namespace sotc {

Init531d60PostBase5b3cf0Result init_531d60_post_base_5b3cf0(
    const Init531d60PostBase5b3cf0Hooks& hooks) noexcept {
    Init531d60PostBase5b3cf0Result out{};

    // OBS: call 0x5b3cf0 (target is ret-nop)
    if (hooks.call_5b3cf0 != nullptr) {
        hooks.call_5b3cf0();
    }
    ++out.call_5b3cf0_count;

    // Next: lea/vmovups AVX BSS copy @ 0x531e17 — NOT modelled here.
    out.ok = (out.call_5b3cf0_count == 1);
    return out;
}

}  // namespace sotc
