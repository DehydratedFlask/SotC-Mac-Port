#include "vtable_15e2028_slot18_e19cb0.hpp"

namespace sotc {

Vtable15e2028Slot18_e19cb0Result vtable_15e2028_slot18_e19cb0(
    const Vtable15e2028Slot18_e19cb0Cells& cells,
    const Vtable15e2028Slot18_e19cb0Hooks& hooks) noexcept {
    Vtable15e2028Slot18_e19cb0Result out{};

    if (hooks.pre_call_e1b1b0 != nullptr) {
        hooks.pre_call_e1b1b0(cells.self);
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
