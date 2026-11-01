#include "vtable_159c908_slot58_2687b0.hpp"

namespace sotc {

Vtable159c908Slot58_2687b0Result vtable_159c908_slot58_2687b0(
    const Vtable159c908Slot58_2687b0Cells& cells,
    const Vtable159c908Slot58_2687b0Hooks& hooks) noexcept {
    Vtable159c908Slot58_2687b0Result out{};

    if (hooks.pre_call_2687d0 != nullptr) {
        hooks.pre_call_2687d0(cells.self);
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
