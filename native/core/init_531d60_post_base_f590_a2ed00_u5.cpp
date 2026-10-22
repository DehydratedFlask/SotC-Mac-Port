#include "init_531d60_post_base_f590_a2ed00_u5.hpp"

namespace sotc {

Init531d60PostBaseF590A2ed00U5Result init_531d60_post_base_f590_a2ed00_u5(
    const Init531d60PostBaseF590A2ed00U5Hooks& hooks) noexcept {
    Init531d60PostBaseF590A2ed00U5Result out{};

    // OBS: prep mov rdi,rbx @ 0x532555..0x532558 (3 B) NOT modelled.

    // OBS: call 0xf590 (body NOT modelled)
    if (hooks.call_f590 != nullptr) {
        hooks.call_f590();
    }
    ++out.call_f590_count;

    // Next: mov rdi,r15 / call 0xf590 @ 0x53255d — NOT modelled here.
    out.ok = (out.call_f590_count == 1);
    return out;
}

}  // namespace sotc
