#include "ctor_slot_403_5f0160.hpp"

namespace sotc {

Slot403Result init_slot403_5f0160(Slot403Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot403Result out{};
    out.var = init_bool_var_site(objects.var, kSlot403VarName, kSlot403FnTag, concmd, 0, 0);
    if (!out.var.ok) return out;
    out.item = init_toggle_site_26db0(objects.item, objects.var, kSlot403Parent, concmd, menu);
    out.ok = out.var.ok && out.item.ok;
    return out;
}

}  // namespace sotc
