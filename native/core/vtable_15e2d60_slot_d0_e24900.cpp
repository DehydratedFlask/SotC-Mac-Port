#include "vtable_15e2d60_slot_d0_e24900.hpp"

namespace sotc {

Vtable15e2d60SlotD0_e24900Result vtable_15e2d60_slot_d0_e24900(
    const Vtable15e2d60SlotD0_e24900Cells& cells,
    const Vtable15e2d60SlotD0_e24900Hooks& hooks) noexcept {
    Vtable15e2d60SlotD0_e24900Result out{};
    if (cells.sub == nullptr || cells.out == nullptr || hooks.slot28 == nullptr ||
        hooks.slot48 == nullptr || hooks.sub_slot40 == nullptr) {
        return out;  // all five calls and the store are unconditional in the retail body
    }

    void* a = hooks.slot28(cells.self);
    const std::uint32_t index = hooks.slot48(cells.self);
    out.result = hooks.sub_slot40(cells.sub, a, index);
    *cells.out = out.result;

    if (out.result == nullptr) {
        out.status = kVtable15e2d60SlotD0_e24900StatusNull;
        out.early_null = true;
        out.ok = true;
        return out;
    }

    if (hooks.result_slot_b8 == nullptr) {
        return out;  // the status call happens for every non-null result
    }
    const std::int32_t status = hooks.result_slot_b8(out.result, cells.self);
    out.status_hook_called = true;

    if (status == 0) {
        *cells.out = nullptr;
        out.out_cleared = true;
        out.status = 0;
        out.ok = true;
        return out;
    }

    if (hooks.result_slot10 != nullptr) {
        hooks.result_slot10(out.result);
        out.release_hook_called = true;
    }
    *cells.out = nullptr;
    out.out_cleared = true;
    out.status = status;
    out.ok = true;
    return out;
}

}  // namespace sotc
