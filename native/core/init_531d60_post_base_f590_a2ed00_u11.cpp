#include "init_531d60_post_base_f590_a2ed00_u11.hpp"

namespace sotc {

Init531d60PostBaseF590A2ed00U11Result init_531d60_post_base_f590_a2ed00_u11(
    const Init531d60PostBaseF590A2ed00U11Hooks& hooks) noexcept {
    Init531d60PostBaseF590A2ed00U11Result out{};

    // OBS: prep mov rdi,rbx @ 0x5326b7..0x5326ba (3 B) NOT modelled.

    // OBS: call 0xf590 (body NOT modelled)
    if (hooks.call_f590 != nullptr) {
        hooks.call_f590();
    }
    ++out.call_f590_count;

    // Next: mov rdi,r15 / call 0xf590 @ 0x5326bf — NOT modelled here.
    out.ok = (out.call_f590_count == 1);
    return out;
}

}  // namespace sotc
