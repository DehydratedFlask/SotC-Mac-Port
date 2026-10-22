#include "init_531d60_post_base_532ed0.hpp"

namespace sotc {

Init531d60PostBase532ed0Result init_531d60_post_base_532ed0(
    const Init531d60PostBase532ed0Hooks& hooks) noexcept {
    Init531d60PostBase532ed0Result out{};

    // OBS: stack prep 0x532285..0x5322f8 (115 B) NOT modelled — call-site only.

    // OBS: call 0x532ed0 (body NOT modelled)
    if (hooks.call_532ed0 != nullptr) {
        hooks.call_532ed0();
    }
    ++out.call_532ed0_count;

    // Next: movq/virtual-call path @ 0x5322fd — NOT modelled here.
    out.ok = (out.call_532ed0_count == 1);
    return out;
}

}  // namespace sotc
