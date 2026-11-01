#include "vtable_15e1ee0_slot_b0_e19980.hpp"

namespace sotc {

const char* vtable_15e1ee0_slot_b0_e19980_path_name(
    Vtable15e1ee0SlotB0_e19980Path path) noexcept {
    switch (path) {
        case Vtable15e1ee0SlotB0_e19980Path::none:
            return "none";
        case Vtable15e1ee0SlotB0_e19980Path::slow:
            return "slow";
        case Vtable15e1ee0SlotB0_e19980Path::fast:
            return "fast";
    }
    return "unknown";
}

Vtable15e1ee0SlotB0_e19980Result vtable_15e1ee0_slot_b0_e19980(
    const Vtable15e1ee0SlotB0_e19980Args& args,
    const Vtable15e1ee0SlotB0_e19980Cells& cells,
    const Vtable15e1ee0SlotB0_e19980Hooks& hooks) noexcept {
    Vtable15e1ee0SlotB0_e19980Result out{};
    if (cells.canary == nullptr) {
        return out;  // the retail body always saves and compares the canary
    }
    const std::uint64_t saved_canary = *cells.canary;

    const auto alignment_mask = static_cast<std::uintptr_t>(
        kVtable15e1ee0SlotB0_e19980AlignmentMask);
    const bool misaligned3 =
        (reinterpret_cast<std::uintptr_t>(args.arg3) & alignment_mask) != 0;
    const bool misaligned4 =
        (reinterpret_cast<std::uintptr_t>(args.arg4) & alignment_mask) != 0;

    if (misaligned3 || misaligned4) {
        out.path = Vtable15e1ee0SlotB0_e19980Path::slow;
        out.path_modelled = true;
        if (hooks.slow_vcall_d8 != nullptr) {
            hooks.slow_vcall_d8(args.arg2, hooks.slow_buffer);
        }
    } else {
        out.path = Vtable15e1ee0SlotB0_e19980Path::fast;
        out.path_modelled = true;
        if (hooks.fast_helper != nullptr) {
            hooks.fast_helper(args.arg3, args.arg2, args.arg4, args.arg5);
        }
        if (cells.fast_store_target != nullptr) {
            out.stored = true;
            if (cells.fast_store_value != nullptr) {
                *cells.fast_store_target = const_cast<void*>(*cells.fast_store_value);
            }
        }
    }

    out.canary_checked = true;
    if (*cells.canary != saved_canary) {
        out.canary_mismatch = true;
        return out;  // retail branch would call __stack_chk_fail
    }
    out.ok = true;
    return out;
}

}  // namespace sotc
