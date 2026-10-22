#include "init_531d60_post_base_52eb50.hpp"

namespace sotc {

Init531d60PostBase52eb50Result init_531d60_post_base_52eb50(
    const Init531d60PostBase52eb50Hooks& hooks) noexcept {
    Init531d60PostBase52eb50Result out{};

    // OBS: call 0x52eb50 (trampoline body NOT modelled)
    if (hooks.call_52eb50 != nullptr) {
        hooks.call_52eb50();
    }
    ++out.call_52eb50_count;

    // Next: lea rbx @ 0x532036 — NOT modelled here.
    out.ok = (out.call_52eb50_count == 1);
    return out;
}

}  // namespace sotc
