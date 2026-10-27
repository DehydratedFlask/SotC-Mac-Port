#include "ctor_slot_355_7946d0.hpp"

namespace sotc {

Slot355Result init_slot355_7946d0(Slot355Objects& objects, ConCmdRegistry& concmd,
                                  DebugMenuRegistry& menu) noexcept {
    Slot355Result out{};
    out.var_a = init_bool_var_site(objects.var_a, kSlot355NameA, kSlot355FnTag, concmd, 0, 0);
    if (!out.var_a.ok) return out;
    out.var_b = init_bool_var_site(objects.var_b, kSlot355NameB, kSlot355FnTag, concmd, 0, 0);
    if (!out.var_b.ok) return out;
    out.item = init_toggle_site_26ea0(objects.item, objects.var_b, kSlot355Label, kSlot355Text, concmd, menu);
    out.ok = out.var_a.ok && out.var_b.ok && out.item.ok;
    return out;
}

}  // namespace sotc
