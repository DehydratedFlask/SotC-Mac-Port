#include "ctor_slot_463_3e0040.hpp"

namespace sotc {

Slot463Result init_slot463_3e0040(Slot463Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot463Result out{};
    out.var = init_bool_var_site(objects.var, kSlot463Name, kSlot463FnTag, concmd, kSlot463Value, kSlot463Default);
    if (!out.var.ok) return out;
    out.item = init_toggle_site_26db0(objects.item, objects.var, kSlot463Parent, concmd, menu);
    out.ok = out.var.ok && out.item.ok;
    return out;
}

}  // namespace sotc
