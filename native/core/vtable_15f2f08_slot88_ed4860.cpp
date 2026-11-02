#include "vtable_15f2f08_slot88_ed4860.hpp"

namespace sotc {

Vtable15f2f08Slot88_ed4860Result vtable_15f2f08_slot88_ed4860(
    const Vtable15f2f08Slot88_ed4860Cells& cells,
    const Vtable15f2f08Slot88_ed4860Hooks& hooks) noexcept {
    Vtable15f2f08Slot88_ed4860Result out{};
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
