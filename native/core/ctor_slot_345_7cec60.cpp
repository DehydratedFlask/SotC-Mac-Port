#include "ctor_slot_345_7cec60.hpp"

namespace sotc {

Slot345Result init_slot345_7cec60(Slot345Objects& objects, ConCmdRegistry& concmd,
                                  DebugMenuRegistry& menu) noexcept {
    Slot345Result out{};
    out.var = init_bool_var_site(objects.var, kSlot345Name, kSlot345FnTag, concmd, 0, 0);
    if (!out.var.ok) return out;
    out.item = init_toggle_site_26ea0(objects.item, objects.var, kSlot345Label, kSlot345Text, concmd, menu);
    out.ok = out.var.ok && out.item.ok;
    return out;
}

}  // namespace sotc
