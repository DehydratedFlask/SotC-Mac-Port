#include "init_531d60_post_base_a2ed00_u4.hpp"

namespace sotc {

Init531d60PostBaseA2ed00U4Result init_531d60_post_base_a2ed00_u4(
    const Init531d60PostBaseA2ed00U4Hooks& hooks) noexcept {
    Init531d60PostBaseA2ed00U4Result out{};

    // OBS: prep 0x532565..0x5325c6 (97 B: mov rdi,[r13] + stack prep) NOT modelled.

    // OBS: call 0xa2ed00 (body NOT modelled)
    if (hooks.call_a2ed00 != nullptr) {
        hooks.call_a2ed00();
    }
    ++out.call_a2ed00_count;

    // Next: mov rdi,rbx / call 0xf590 @ 0x5325cb — NOT modelled here.
    out.ok = (out.call_a2ed00_count == 1);
    return out;
}

}  // namespace sotc
