#include "init_531d60_post_base_a2ed00.hpp"

namespace sotc {

Init531d60PostBaseA2ed00Result init_531d60_post_base_a2ed00(
    const Init531d60PostBaseA2ed00Hooks& hooks) noexcept {
    Init531d60PostBaseA2ed00Result out{};

    // OBS: prep 0x5323f5..0x53245d (104 B: mov rdi,[r13] + stack prep) NOT modelled.

    // OBS: call 0xa2ed00 (body NOT modelled)
    if (hooks.call_a2ed00 != nullptr) {
        hooks.call_a2ed00();
    }
    ++out.call_a2ed00_count;

    // Next: mov rdi,rbx / call 0xf590 @ 0x532462 — NOT modelled here.
    out.ok = (out.call_a2ed00_count == 1);
    return out;
}

}  // namespace sotc
