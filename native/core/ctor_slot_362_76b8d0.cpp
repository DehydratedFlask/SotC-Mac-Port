#include "ctor_slot_362_76b8d0.hpp"

namespace sotc {

Slot362Result init_slot362_76b8d0(Slot362Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot362Result out{};
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot362Var0Name, kSlot362Var0NameLength,
                                                "", 0, kSlot362BoolFnTag, concmd, 0, 0);
    out.var1 = lane_m6::init_bool_var_literal(objects.var1, kSlot362Var1Name, kSlot362Var1NameLength,
                                                "", 0, kSlot362BoolFnTag, concmd, 0, 0);
    out.var2 = lane_m6::init_bool_var_literal(objects.var2, kSlot362Var2Name, kSlot362Var2NameLength,
                                                "", 0, kSlot362BoolFnTag, concmd, 0, 0);
    objects.record0.name = kSlot362Record0Name;                      // mov [0x3253868], name
    objects.record0.hash = fnv_cstr_52a990(kSlot362Record0Name);   // call 0x52a990 ; mov [cell+8], eax
    out.records_written = 1;
    out.ok = out.var0.ok && out.var1.ok && out.var2.ok;
    return out;
}

}  // namespace sotc
