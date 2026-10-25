#include "init_531d60_post_base_532ed0_u2.hpp"

namespace sotc {

Init531d60PostBase532ed0U2Result init_531d60_post_base_532ed0_u2(
    const Init531d60PostBase532ed0U2Hooks& hooks) noexcept {
    Init531d60PostBase532ed0U2Result out{};

    // OBS: stack prep 0x53230b..0x532364 (89 B) NOT modelled — call-site only.

    // OBS: call 0x532ed0 (body NOT modelled)
    if (hooks.call_532ed0 != nullptr) {
        hooks.call_532ed0();
    }
    ++out.call_532ed0_count;

    // Next: movq/virtual-call path @ 0x532369 — NOT modelled here.
    out.ok = (out.call_532ed0_count == 1);
    return out;
}

}  // namespace sotc
