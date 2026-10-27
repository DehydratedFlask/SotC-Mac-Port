#include "ctor_slot_350_7af0b0.hpp"

namespace sotc {

Slot350Result init_slot350_7af0b0(Slot350Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot350Result out{};
    out.var_a = init_float_var_static(objects.var_a, kSlot350SpecA, concmd);
    if (!out.var_a.ok) return out;
    out.var_b = init_float_var_static(objects.var_b, kSlot350SpecB, concmd);
    out.ok = out.var_a.ok && out.var_b.ok;
    return out;
}

}  // namespace sotc
