#include "init_531d60_post_base_vcall18_532b88_u1.hpp"

namespace sotc {

Init531d60PostBaseVcall18_532b88_U1Result init_531d60_post_base_vcall18_532b88_u1(
    const Init531d60PostBaseVcall18_532b88_U1Hooks& hooks) noexcept {
    Init531d60PostBaseVcall18_532b88_U1Result out{};

    if (hooks.call_vslot18 != nullptr) {
        hooks.call_vslot18();
    }
    ++out.call_vslot18_count;

    // Next: add rbx,0x58 @ 0x532b8b — NOT modelled here.
    out.ok = (out.call_vslot18_count == 1);
    return out;
}

}  // namespace sotc
