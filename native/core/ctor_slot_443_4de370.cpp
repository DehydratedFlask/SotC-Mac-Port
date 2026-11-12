#include "ctor_slot_443_4de370.hpp"

namespace sotc {

Slot443Result init_slot443_4de370(Slot443Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot443Result out{};
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot443Var0Name, kSlot443Var0NameLength,
                                                "", 0, kSlot443BoolFnTag, concmd, 0, 0);
    out.var1 = lane_m6::init_bool_var_literal(objects.var1, kSlot443Var1Name, kSlot443Var1NameLength,
                                                "", 0, kSlot443BoolFnTag, concmd, 0, 0);
    out.group0 = family_group_item_26c10(objects.group0, kSlot443Group0Parent, kSlot443Group0ParentLength,
                                          kSlot443Group0Caption, kSlot443Group0CaptionLength, kSlot443GroupFlags, concmd, menu);
    out.item0 = lane_m6::init_toggle_site_target(objects.item0, debug_menu_target_for(objects.var0),
                                                  kSlot443Item0Label, kSlot443Item0LabelLength, kSlot443Item0Text,
                                                  kSlot443Item0TextLength, concmd, menu);
    out.item1 = lane_m6::init_toggle_site_target(objects.item1, debug_menu_target_for(objects.var1),
                                                  kSlot443Item1Label, kSlot443Item1LabelLength, kSlot443Item1Text,
                                                  kSlot443Item1TextLength, concmd, menu);
    out.ok = out.var0.ok && out.var1.ok && out.group0.ok() && out.item0.ok && out.item1.ok;
    return out;
}

}  // namespace sotc
