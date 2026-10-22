#include "init_531d60_post_base_a2ed00_u3.hpp"

namespace sotc {

Init531d60PostBaseA2ed00U3Result init_531d60_post_base_a2ed00_u3(
    const Init531d60PostBaseA2ed00U3Hooks& hooks) noexcept {
    Init531d60PostBaseA2ed00U3Result out{};

    // OBS: prep 0x5324e8..0x532550 (104 B: mov rdi,[r13] + stack prep) NOT modelled.

    // OBS: call 0xa2ed00 (body NOT modelled)
    if (hooks.call_a2ed00 != nullptr) {
        hooks.call_a2ed00();
    }
    ++out.call_a2ed00_count;

    // Next: mov rdi,rbx / call 0xf590 @ 0x532555 — NOT modelled here.
    out.ok = (out.call_a2ed00_count == 1);
    return out;
}

}  // namespace sotc
