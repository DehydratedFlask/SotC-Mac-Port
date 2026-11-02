#include "vtable_15953f8_slot18_31310.hpp"

#include "alloc_wrappers_17070.hpp"

namespace sotc {

Vtable15953f8Slot18_31310Result vtable_15953f8_slot18_31310(
    const Vtable15953f8Slot18_31310Cells& cells,
    const Vtable15953f8Slot18_31310Hooks& hooks) noexcept {
    Vtable15953f8Slot18_31310Result out{};
    if (cells.vtable_field == nullptr || cells.op_handle == nullptr || cells.mode == nullptr) {
        return out;  // the vtable store, the wait argument and the mode read are unconditional
    }

    out.vtable_written =
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(kVtable15953f8Slot18InstalledTable));
    *cells.vtable_field = out.vtable_written;

    bool finished = hooks.sub_init != nullptr && hooks.sub_init(cells.self);
    while (!finished) {
        if (out.waits >= kVtable15953f8Slot18MaxWaits) {
            // The retail loop would spin forever; report it rather than reproducing the hang.
            out.wait_overflow = true;
            return out;
        }
        if (hooks.wait != nullptr) {
            hooks.wait(*cells.op_handle);
        }
        ++out.waits;
        finished = hooks.positioned_read != nullptr && hooks.positioned_read(cells.self);
    }

    if (hooks.plain_read != nullptr) {
        hooks.plain_read(cells.self);
    }
    out.plain_read_called = true;

    if (*cells.mode == kVtable15953f8Slot18ModeOne) {
        if (hooks.mode_one != nullptr) {
            hooks.mode_one(cells.self);
        }
        out.mode_one_called = true;
    }

    if (hooks.finish != nullptr) {
        hooks.finish(cells.self);
    }
    out.finish_called = true;

    AllocDispatchContract contract{};
    const AllocWrapperResult freed = free_170b0(cells.self, contract);
    out.free_tag = freed.tag;
    out.freed = freed.ok;
    out.ok = freed.ok;
    return out;
}

}  // namespace sotc
