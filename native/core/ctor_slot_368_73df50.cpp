#include "ctor_slot_368_73df50.hpp"

namespace sotc {

Slot368Result init_slot368_73df50(Slot368Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot368Result out{};
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot368Var0Name, kSlot368Var0NameLength,
                                                "", 0, kSlot368BoolFnTag, concmd, 1, 1);
    out.var1 = lane_m6::init_bool_var_literal(objects.var1, kSlot368Var1Name, kSlot368Var1NameLength,
                                                "", 0, kSlot368BoolFnTag, concmd, 1, 1);
    out.var2 = lane_m6::init_bool_var_literal(objects.var2, kSlot368Var2Name, kSlot368Var2NameLength,
                                                "", 0, kSlot368BoolFnTag, concmd, 0, 0);
    out.item0 = lane_m6::init_toggle_site_target(objects.item0, debug_menu_target_for(objects.var0),
                                                  kSlot368Item0Label, kSlot368Item0LabelLength, kSlot368Item0Text,
                                                  kSlot368Item0TextLength, concmd, menu);
    out.item1 = lane_m6::init_toggle_site_target(objects.item1, debug_menu_target_for(objects.var1),
                                                  kSlot368Item1Label, kSlot368Item1LabelLength, kSlot368Item1Text,
                                                  kSlot368Item1TextLength, concmd, menu);
    out.item2 = lane_m6::init_toggle_site_target(objects.item2, debug_menu_target_for(objects.var2),
                                                  kSlot368Item2Label, kSlot368Item2LabelLength, kSlot368Item2Text,
                                                  kSlot368Item2TextLength, concmd, menu);
    for (std::size_t i = 0; i < kSlot368StaticStoreCount; ++i) objects.statics[i] = kSlot368StaticStores[i].bits;   // movl $imm, [cell]
    out.statics_written = kSlot368StaticStoreCount;
    out.ok = out.var0.ok && out.var1.ok && out.var2.ok && out.item0.ok && out.item1.ok && out.item2.ok && out.statics_written == kSlot368StaticStoreCount;
    return out;
}

}  // namespace sotc
