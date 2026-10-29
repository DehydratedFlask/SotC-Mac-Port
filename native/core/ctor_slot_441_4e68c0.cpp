#include "ctor_slot_441_4e68c0.hpp"

namespace sotc {

Slot441Result init_slot441_4e68c0(Slot441Objects& objects, ConCmdRegistry& concmd,
                         DebugMenuRegistry& menu) noexcept {
    Slot441Result out{};
    out.var = init_bool_var_site(objects.var, kSlot441VarName, kSlot441FnTag, concmd, 0, 0);
    if (!out.var.ok) return out;
    out.item = init_toggle_site_26db0(objects.item, objects.var, kSlot441Parent, concmd, menu);
    out.ok = out.var.ok && out.item.ok;
    return out;
}

}  // namespace sotc
