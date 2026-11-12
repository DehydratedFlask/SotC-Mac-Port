#include "ctor_slot_512_1d5e80.hpp"

namespace sotc {

Slot512Result init_slot512_1d5e80(Slot512Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot512Result out{};
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot512Var0Name, kSlot512Var0NameLength,
                                                "", 0, kSlot512BoolFnTag, concmd, 1, 1);
    out.group0 = family_group_item_26c10(objects.group0, kSlot512Group0Parent, kSlot512Group0ParentLength,
                                          kSlot512Group0Caption, kSlot512Group0CaptionLength, kSlot512GroupFlags, concmd, menu);
    out.item0 = lane_m6::init_toggle_site_target(objects.item0, debug_menu_target_for(objects.var0),
                                                  kSlot512Item0Label, kSlot512Item0LabelLength, kSlot512Item0Text,
                                                  kSlot512Item0TextLength, concmd, menu);
    out.var1 = lane_m6::init_bool_var_literal(objects.var1, kSlot512Var1Name, kSlot512Var1NameLength,
                                                "", 0, kSlot512BoolFnTag, concmd, 0, 0);
    out.item1 = lane_m6::init_toggle_site_target(objects.item1, debug_menu_target_for(objects.var1),
                                                  kSlot512Item1Label, kSlot512Item1LabelLength, kSlot512Item1Text,
                                                  kSlot512Item1TextLength, concmd, menu);
    out.ok = out.var0.ok && out.group0.ok() && out.item0.ok && out.var1.ok && out.item1.ok;
    return out;
}

}  // namespace sotc
