#include "init_531d60_post_base_f90310.hpp"

namespace sotc {

Init531d60PostBaseF90310Result init_531d60_post_base_f90310(
    const Init531d60PostBaseF90310Hooks& hooks) noexcept {
    Init531d60PostBaseF90310Result out{};

    // OBS: call 0xf90310 (splash-hide PLT; body NOT modelled)
    if (hooks.call_f90310 != nullptr) {
        hooks.call_f90310();
    }
    ++out.call_f90310_count;

    // Next: vmovups ymm BSS seed @ 0x531e4b — E457; not this fragment.
    out.ok = (out.call_f90310_count == 1);
    return out;
}

}  // namespace sotc
