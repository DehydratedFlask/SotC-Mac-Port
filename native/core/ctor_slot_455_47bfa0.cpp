#include "ctor_slot_455_47bfa0.hpp"

namespace sotc {

Slot455Result init_slot455_47bfa0(Slot455Objects& objects, ConCmdRegistry& concmd,
                            DebugMenuRegistry& menu) noexcept {
    Slot455Result out{};
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot455Var0Name, kSlot455Var0NameLength, "", 0, kSlot455FnTag,
                                              concmd, 1, 1);
    if (!out.var0.ok) return out;
    out.var1 = lane_m6::init_bool_var_literal(objects.var1, kSlot455Var1Name, kSlot455Var1NameLength, "", 0, kSlot455FnTag,
                                              concmd, 0, 0);
    if (!out.var1.ok) return out;
    out.group = family_group_item_26c10(objects.group, kSlot455GroupParent, kSlot455GroupParentLength, kSlot455GroupCaption,
                                        kSlot455GroupCaptionLength, kSlot455GroupFlags, concmd, menu);
    if (!out.group.ok()) return out;
    out.item0 = init_toggle_site_26db0(objects.item0, objects.var0, kSlot455GroupCaption, concmd, menu);
    if (!out.item0.ok) return out;
    out.item1 = init_toggle_site_26db0(objects.item1, objects.var1, kSlot455GroupCaption, concmd, menu);
    out.ok = out.var0.ok && out.var1.ok && out.group.ok() && out.item0.ok && out.item1.ok;
    return out;
}

}  // namespace sotc
