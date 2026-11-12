#include "ctor_slot_449_4b0200.hpp"

namespace sotc {

Slot449Result init_slot449_4b0200(Slot449Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot449Result out{};
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot449Var0Name, kSlot449Var0NameLength,
                                                "", 0, kSlot449BoolFnTag, concmd, 0, 0);
    out.item0 = lane_m6::init_toggle_site_target(objects.item0, debug_menu_target_for(objects.var0),
                                                  kSlot449Item0Label, kSlot449Item0LabelLength, kSlot449Item0Text,
                                                  kSlot449Item0TextLength, concmd, menu);
    out.var1 = lane_m6::init_bool_var_literal(objects.var1, kSlot449Var1Name, kSlot449Var1NameLength,
                                                "", 0, kSlot449BoolFnTag, concmd, 0, 0);
    out.item1 = lane_m6::init_toggle_site_target(objects.item1, debug_menu_target_for(objects.var1),
                                                  kSlot449Item1Label, kSlot449Item1LabelLength, kSlot449Item1Text,
                                                  kSlot449Item1TextLength, concmd, menu);
    out.ok = out.var0.ok && out.var1.ok && out.item0.ok && out.item1.ok;
    return out;
}

}  // namespace sotc
