#include "init_531d60_post_base_vcall70_532c19_u1.hpp"

namespace sotc {

Init531d60PostBaseVcall70_532c19_U1Result init_531d60_post_base_vcall70_532c19_u1(
    const Init531d60PostBaseVcall70_532c19_U1Cells& cells,
    const Init531d60PostBaseVcall70_532c19_U1Hooks& hooks) noexcept {
    Init531d60PostBaseVcall70_532c19_U1Result out{};

    if (hooks.call_vslot70 != nullptr) {
        hooks.call_vslot70(cells.self, cells.field, cells.tag);
    }
    ++out.call_vslot70_532c19_count;

    // Next: E616 mov rbx,[r14+0x3ec0] @ 0x532c1c — separate unit.
    out.ok = (out.call_vslot70_532c19_count == 1);
    return out;
}

}  // namespace sotc
