#include "ctor_slot_388_6a8c60.hpp"

namespace sotc {

Slot388Result init_slot388_6a8c60(Slot388Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot388Result out{};
    // 0x6a8cf2..0x6a8d92: bool variable, empty description, caller tag 0x15cc6f8, value = default 1, atexit 0x20b0.
    if (!lane_m6::init_bool_var_literal(objects.bvar, kSlot388BoolName, kSlot388BoolNameLength, "", 0, kSlot388BoolFnTag, concmd, kSlot388BoolValue,
                                        kSlot388BoolValue).ok) {
        return out;
    }
    ++out.rows_done;
    // 0x6a8df4..0x6a8e16: 0x26ea0 toggle over the bool variable (explicit text), atexit 0x272d0.
    if (!family_toggle_item_26ea0(objects.item, kSlot388ToggleParent, kSlot388ToggleParentLength, kSlot388ToggleText, kSlot388ToggleTextLength,
                                  debug_menu_target_for(objects.bvar), kSlot388ItemFlags, concmd, menu).ok()) {
        return out;
    }
    ++out.rows_done;
    // 0x6a8e2c..0x6a8f12: the console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], atexit 0x4b0).
    const FConCmdCommandSpec cmd{kSlot388CommandName, kSlot388CommandDescription, kSlot388CommandDescriptionLength, kSlot388HandlerTag};
    out.command = init_fconcmd_object(objects.command, cmd, concmd);
    if (!out.command.ok) return out;
    ++out.rows_done;
    out.ok = true;
    return out;
}

}  // namespace sotc
