#include "ctor_slot_477_3270d0.hpp"

namespace sotc {

Slot477Result init_slot477_3270d0(Slot477Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot477Result out{};
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot477Var0Name, kSlot477Var0NameLength,
                                                kSlot477Var0Description, kSlot477Var0DescriptionLength, kSlot477BoolFnTag, concmd, 0, 0);
    out.var1 = lane_m6::init_bool_var_literal(objects.var1, kSlot477Var1Name, kSlot477Var1NameLength,
                                                kSlot477Var1Description, kSlot477Var1DescriptionLength, kSlot477BoolFnTag, concmd, 0, 0);
    out.var2 = lane_m6::init_bool_var_literal(objects.var2, kSlot477Var2Name, kSlot477Var2NameLength,
                                                kSlot477Var2Description, kSlot477Var2DescriptionLength, kSlot477BoolFnTag, concmd, 0, 0);
    out.var3 = lane_m6::init_bool_var_literal(objects.var3, kSlot477Var3Name, kSlot477Var3NameLength,
                                                kSlot477Var3Description, kSlot477Var3DescriptionLength, kSlot477BoolFnTag, concmd, 0, 0);
    out.ok = out.var0.ok && out.var1.ok && out.var2.ok && out.var3.ok;
    return out;
}

}  // namespace sotc
