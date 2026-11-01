#include "vtable_1598e10_slot18_1bf710.hpp"

namespace sotc {

Vtable1598e10Slot18_1bf710Result vtable_1598e10_slot18_1bf710(
    const Vtable1598e10Slot18_1bf710Cells& cells,
    const Vtable1598e10Slot18_1bf710Hooks& hooks) noexcept {
    Vtable1598e10Slot18_1bf710Result out{};

    if (hooks.pre_call_1c0010 != nullptr) {
        hooks.pre_call_1c0010(cells.self);
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
