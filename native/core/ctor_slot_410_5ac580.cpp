#include "ctor_slot_410_5ac580.hpp"

namespace sotc {

Slot410Result init_slot410_5ac580(Slot410Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot410Result out{};
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot410Var0Name, kSlot410Var0NameLength,
                                                "", 0, kSlot410BoolFnTag, concmd, 1, 1);
    out.var1 = lane_m6::init_bool_var_literal(objects.var1, kSlot410Var1Name, kSlot410Var1NameLength,
                                                "", 0, kSlot410BoolFnTag, concmd, 0, 0);
    out.var2 = lane_m6::init_bool_var_literal(objects.var2, kSlot410Var2Name, kSlot410Var2NameLength,
                                                "", 0, kSlot410BoolFnTag, concmd, 0, 0);
    out.ok = out.var0.ok && out.var1.ok && out.var2.ok;
    return out;
}

}  // namespace sotc
