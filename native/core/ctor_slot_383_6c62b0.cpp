#include "ctor_slot_383_6c62b0.hpp"

namespace sotc {

Slot383Result init_slot383_6c62b0(Slot383Objects& objects, ConCmdRegistry& concmd,
                                  DebugMenuRegistry& menu) noexcept {
    Slot383Result out{};
    out.var = init_bool_var_site(objects.var, kSlot383VarName, kSlot383FnTag, concmd, 0, 0);
    if (!out.var.ok) return out;
    out.item = init_toggle_site_26db0(objects.item, objects.var, kSlot383Parent, concmd, menu);
    out.ok = out.var.ok && out.item.ok;
    return out;
}

}  // namespace sotc
