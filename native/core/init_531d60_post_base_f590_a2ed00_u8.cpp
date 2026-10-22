#include "init_531d60_post_base_f590_a2ed00_u8.hpp"

namespace sotc {

Init531d60PostBaseF590A2ed00U8Result init_531d60_post_base_f590_a2ed00_u8(
    const Init531d60PostBaseF590A2ed00U8Hooks& hooks) noexcept {
    Init531d60PostBaseF590A2ed00U8Result out{};

    // OBS: prep mov rdi,r15 @ 0x5325d3..0x5325d6 (3 B) NOT modelled.

    // OBS: call 0xf590 (body NOT modelled)
    if (hooks.call_f590 != nullptr) {
        hooks.call_f590();
    }
    ++out.call_f590_count;

    // Next: mov rdi,[r13] / stack prep / call 0xa2ed00 @ 0x5325db — NOT modelled here.
    out.ok = (out.call_f590_count == 1);
    return out;
}

}  // namespace sotc
