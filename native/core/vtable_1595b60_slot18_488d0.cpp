#include "vtable_1595b60_slot18_488d0.hpp"

namespace sotc {

Vtable1595b60Slot18_488d0Result vtable_1595b60_slot18_488d0(
    const Vtable1595b60Slot18_488d0Cells& cells,
    const Vtable1595b60Slot18_488d0Hooks& hooks) noexcept {
    Vtable1595b60Slot18_488d0Result out{};

    if (hooks.pre_call_48790 != nullptr) {
        hooks.pre_call_48790(cells.self);
    }
    out.pre_call_modelled = true;  // the call is unconditional in the retail body

    AllocDispatchContract contract{};
    if (hooks.alloc != nullptr) {
        contract = *hooks.alloc;
    }
    const AllocWrapperResult freed = free_170b0(cells.self, contract);
    out.free_tag = freed.tag;
    out.freed = freed.ok;
    out.ok = freed.ok;
    return out;
}

}  // namespace sotc
