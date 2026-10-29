#include "ctor_slot_389_695a50.hpp"

namespace sotc {

Slot389Result init_slot389_695a50(Slot389Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot389Result out{};
    out.var = init_bool_var_site(objects.var, kSlot389VarName, kSlot389FnTag, concmd, 1, 1);
    if (!out.var.ok) return out;
    out.item = init_toggle_site_26db0(objects.item, objects.var, kSlot389Parent, concmd, menu);
    out.ok = out.var.ok && out.item.ok;
    return out;
}

}  // namespace sotc
