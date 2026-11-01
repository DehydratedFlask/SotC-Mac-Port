#include "vtable_15ed2a0_slot_c0_e26f90.hpp"

namespace sotc {

Vtable15ed2a0SlotC0_e26f90Result vtable_15ed2a0_slot_c0_e26f90(
    const Vtable15ed2a0SlotC0_e26f90Cells& cells,
    const Vtable15ed2a0SlotC0_e26f90Hooks& hooks) noexcept {
    Vtable15ed2a0SlotC0_e26f90Result out{};
    if (cells.canary == nullptr || cells.buffer == nullptr || cells.local == nullptr ||
        hooks.slot78 == nullptr || hooks.slot90 == nullptr) {
        return out;  // the frame, both calls and the canary are unconditional
    }
    const std::uint64_t saved_canary = *cells.canary;

    out.fetched = hooks.slot78(cells.self);
    const auto name = reinterpret_cast<const char*>(
        static_cast<std::uintptr_t>(kVtable15ed2a0SlotC0_e26f90NameRva));
    out.status = hooks.slot90(cells.self, name, cells.buffer);
    out.status_called = true;

    if (out.status != 0) {
        out.early_zero = true;  // `test eax,eax; jne` -> xor eax,eax
        out.status = 0;
    } else {
        if (hooks.helper == nullptr) {
            return out;  // the helper call is unconditional on this path
        }
        out.status = hooks.helper(cells.self, cells.arg2, cells.arg3, cells.buffer, cells.local);
        out.helper_called = true;
    }

    out.canary_checked = true;
    if (*cells.canary != saved_canary) {
        out.canary_mismatch = true;
        return out;
    }
    out.ok = true;
    return out;
}

}  // namespace sotc
