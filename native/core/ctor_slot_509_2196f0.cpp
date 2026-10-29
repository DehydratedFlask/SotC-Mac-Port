#include "ctor_slot_509_2196f0.hpp"

namespace sotc {

Slot509Result init_slot509_2196f0(Slot509Objects& objects, ConCmdRegistry& concmd,
                                  DebugMenuRegistry& menu) noexcept {
    Slot509Result out{};
    out.var = init_bool_var_site(objects.var, kSlot509Name, kSlot509FnTag, concmd, 0, 0);
    if (!out.var.ok) return out;
    out.item = init_toggle_site_26ea0(objects.item, objects.var, kSlot509Label, kSlot509Text, concmd, menu);
    out.ok = out.var.ok && out.item.ok;
    return out;
}

}  // namespace sotc
