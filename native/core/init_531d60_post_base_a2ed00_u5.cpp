#include "init_531d60_post_base_a2ed00_u5.hpp"

namespace sotc {

Init531d60PostBaseA2ed00U5Result init_531d60_post_base_a2ed00_u5(
    const Init531d60PostBaseA2ed00U5Hooks& hooks) noexcept {
    Init531d60PostBaseA2ed00U5Result out{};

    // OBS: prep 0x5325db..0x53263f (100 B: mov rdi,[r13] + stack prep) NOT modelled.

    // OBS: call 0xa2ed00 (body NOT modelled)
    if (hooks.call_a2ed00 != nullptr) {
        hooks.call_a2ed00();
    }
    ++out.call_a2ed00_count;

    // Next: mov rdi,rbx / call 0xf590 @ 0x532644 — NOT modelled here.
    out.ok = (out.call_a2ed00_count == 1);
    return out;
}

}  // namespace sotc
