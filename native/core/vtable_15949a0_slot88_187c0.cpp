#include "vtable_15949a0_slot88_187c0.hpp"

namespace sotc {

Vtable15949a0Slot88_187c0Result vtable_15949a0_slot88_187c0(
    const Vtable15949a0Slot88_187c0Cells& cells,
    const Vtable15949a0Slot88_187c0Hooks& hooks) noexcept {
    Vtable15949a0Slot88_187c0Result out{};

    if (hooks.pre_call_47620 != nullptr) {
        hooks.pre_call_47620(cells.self);
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
