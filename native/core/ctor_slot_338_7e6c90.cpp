#include "ctor_slot_338_7e6c90.hpp"

namespace sotc {

Slot338Result init_slot338_7e6c90(Slot338Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot338Result out{};
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot338Var0Name, kSlot338Var0NameLength,
                                                "", 0, kSlot338BoolFnTag, concmd, 0, 0);
    out.var1 = lane_m6::init_bool_var_literal(objects.var1, kSlot338Var1Name, kSlot338Var1NameLength,
                                                "", 0, kSlot338BoolFnTag, concmd, 0, 0);
    out.var2 = lane_m6::init_bool_var_literal(objects.var2, kSlot338Var2Name, kSlot338Var2NameLength,
                                                "", 0, kSlot338BoolFnTag, concmd, 0, 0);
    out.var3 = lane_m6::init_bool_var_literal(objects.var3, kSlot338Var3Name, kSlot338Var3NameLength,
                                                "", 0, kSlot338BoolFnTag, concmd, 0, 0);
    out.var4 = lane_m6::init_bool_var_literal(objects.var4, kSlot338Var4Name, kSlot338Var4NameLength,
                                                "", 0, kSlot338BoolFnTag, concmd, 0, 0);
    out.var5 = lane_m6::init_bool_var_literal(objects.var5, kSlot338Var5Name, kSlot338Var5NameLength,
                                                "", 0, kSlot338BoolFnTag, concmd, 0, 0);
    out.var6 = lane_m6::init_bool_var_literal(objects.var6, kSlot338Var6Name, kSlot338Var6NameLength,
                                                "", 0, kSlot338BoolFnTag, concmd, 0, 0);
    out.var7 = lane_m6::init_bool_var_literal(objects.var7, kSlot338Var7Name, kSlot338Var7NameLength,
                                                "", 0, kSlot338BoolFnTag, concmd, 0, 0);
    out.var8 = lane_m6::init_bool_var_literal(objects.var8, kSlot338Var8Name, kSlot338Var8NameLength,
                                                "", 0, kSlot338BoolFnTag, concmd, 0, 0);
    out.ok = out.var0.ok && out.var1.ok && out.var2.ok && out.var3.ok && out.var4.ok && out.var5.ok && out.var6.ok && out.var7.ok && out.var8.ok;
    return out;
}

}  // namespace sotc
