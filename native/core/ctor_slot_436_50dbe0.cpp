#include "ctor_slot_436_50dbe0.hpp"

namespace sotc {

Slot436Result init_slot436_50dbe0(Slot436Objects& objects, ConCmdRegistry& concmd,
                         DebugMenuRegistry& menu) noexcept {
    Slot436Result out{};
    out.var = init_bool_var_site(objects.var, kSlot436VarName, kSlot436FnTag, concmd, 0, 0);
    if (!out.var.ok) return out;
    out.item = init_toggle_site_26db0(objects.item, objects.var, kSlot436Parent, concmd, menu);
    out.ok = out.var.ok && out.item.ok;
    return out;
}

}  // namespace sotc
