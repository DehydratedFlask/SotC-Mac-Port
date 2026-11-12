#include "ctor_slot_423_55f080.hpp"

namespace sotc {

Slot423Result init_slot423_55f080(Slot423Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot423Result out{};
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot423Var0Name, kSlot423Var0NameLength,
                                                "", 0, kSlot423BoolFnTag, concmd, 0, 0);
    out.var1 = lane_m6::init_bool_var_literal(objects.var1, kSlot423Var1Name, kSlot423Var1NameLength,
                                                "", 0, kSlot423BoolFnTag, concmd, 0, 0);
    out.var2 = lane_m6::init_bool_var_literal(objects.var2, kSlot423Var2Name, kSlot423Var2NameLength,
                                                "", 0, kSlot423BoolFnTag, concmd, 0, 0);
    out.var3 = lane_m6::init_bool_var_literal(objects.var3, kSlot423Var3Name, kSlot423Var3NameLength,
                                                "", 0, kSlot423BoolFnTag, concmd, 0, 0);
    objects.record0.name = kSlot423Record0Name;                      // mov [0x2f06388], name
    objects.record0.hash = fnv_cstr_52a990(kSlot423Record0Name);   // call 0x52a990 ; mov [cell+8], eax
    objects.record1.name = kSlot423Record1Name;                      // mov [0x2f06398], name
    objects.record1.hash = fnv_cstr_52a990(kSlot423Record1Name);   // call 0x52a990 ; mov [cell+8], eax
    out.records_written = 2;
    out.ok = out.var0.ok && out.var1.ok && out.var2.ok && out.var3.ok;
    return out;
}

}  // namespace sotc
