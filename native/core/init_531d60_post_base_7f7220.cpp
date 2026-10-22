#include "init_531d60_post_base_7f7220.hpp"

namespace sotc {

Init531d60PostBase7f7220Result init_531d60_post_base_7f7220(
    const Init531d60PostBase7f7220Hooks& hooks) noexcept {
    Init531d60PostBase7f7220Result out{};

    // OBS: call 0x7f7220 (staffroll helper body NOT modelled)
    if (hooks.call_7f7220 != nullptr) {
        hooks.call_7f7220();
    }
    ++out.call_7f7220_count;

    // Next: call 0x56fad0 @ 0x531e41 — NOT modelled here.
    out.ok = (out.call_7f7220_count == 1);
    return out;
}

}  // namespace sotc
