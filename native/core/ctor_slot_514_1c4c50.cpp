#include "ctor_slot_514_1c4c50.hpp"

namespace sotc {

Slot514Result init_slot514_1c4c50(Slot514Objects& objects, ConCmdRegistry& concmd,
                            DebugMenuRegistry& menu) noexcept {
    Slot514Result out{};
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot514Var0Name, kSlot514Var0NameLength, "", 0, kSlot514FnTag,
                                              concmd, 0, 0);
    if (!out.var0.ok) return out;
    out.var1 = lane_m6::init_bool_var_literal(objects.var1, kSlot514Var1Name, kSlot514Var1NameLength, "", 0, kSlot514FnTag,
                                              concmd, 1, 1);
    if (!out.var1.ok) return out;
    out.item0 = init_toggle_site_26db0(objects.item0, objects.var0, kSlot514Parent, concmd, menu);
    if (!out.item0.ok) return out;
    out.item1 = init_toggle_site_26db0(objects.item1, objects.var1, kSlot514Parent, concmd, menu);
    out.ok = out.var0.ok && out.var1.ok && out.item0.ok && out.item1.ok;
    return out;
}

}  // namespace sotc
