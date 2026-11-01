#include "vtable_15d1678_slot280_a460f0.hpp"

namespace sotc {

Vtable15d1678Slot280_a460f0Result vtable_15d1678_slot280_a460f0(
    const Vtable15d1678Slot280_a460f0Cells& cells,
    const Vtable15d1678Slot280_a460f0Hooks& hooks) noexcept {
    Vtable15d1678Slot280_a460f0Result out{};
    if (cells.vtable_field == nullptr || cells.helper_field == nullptr) {
        return out;  // the store and the helper read are unconditional in the retail body
    }

    out.vtable_written =
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(kVtable15d1678Slot280_a460f0InstalledTable));
    *cells.vtable_field = out.vtable_written;

    void* helper = const_cast<void*>(*cells.helper_field);
    if (helper == nullptr) {
        return out;  // `call *0x18(%rax)` would fault on a null helper
    }
    if (hooks.helper_vcall18 != nullptr) {
        hooks.helper_vcall18(helper);
    }
    out.helper_called = true;

    if (hooks.post_call_20b50 != nullptr) {
        hooks.post_call_20b50(cells.self);
    }
    out.post_called = true;

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
