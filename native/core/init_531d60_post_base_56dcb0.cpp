#include "init_531d60_post_base_56dcb0.hpp"

namespace sotc {

Init531d60PostBase56dcb0Result init_531d60_post_base_56dcb0(
    const Init531d60PostBase56dcb0Hooks& hooks) noexcept {
    Init531d60PostBase56dcb0Result out{};

    // OBS: call 0x56dcb0
    if (hooks.call_56dcb0 != nullptr) {
        hooks.call_56dcb0();
    }
    ++out.call_56dcb0_count;

    // Next: call 0x56dca0 @ 0x531df9 — E406 remainder ret-nop; not this fragment.
    out.ok = (out.call_56dcb0_count == 1);
    return out;
}

}  // namespace sotc
