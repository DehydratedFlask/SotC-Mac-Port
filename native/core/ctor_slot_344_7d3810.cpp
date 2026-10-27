#include "ctor_slot_344_7d3810.hpp"

namespace sotc {

Slot344Result init_slot344_7d3810(Slot344Objects& objects, ConCmdRegistry& concmd,
                                  DebugMenuRegistry& menu) noexcept {
    Slot344Result out{};
    init_flag_scale_cell(objects.cell_a);
    init_flag_scale_cell(objects.cell_b);
    out.var = init_bool_var_site(objects.var, kSlot344VarName, kSlot344FnTag, concmd, 0, 0);
    if (!out.var.ok) return out;
    out.item = init_toggle_site_26db0(objects.item, objects.var, kSlot344Parent, concmd, menu);
    if (!out.item.ok) return out;
    const FConCmdCommandSpec spec{kSlot344CommandName, kSlot344CommandDescription, kSlot344CommandDescriptionLength,
                                  kSlot344HandlerTag};
    out.command = init_fconcmd_object(objects.command, spec, concmd);
    out.ok = out.var.ok && out.item.ok && out.command.ok;
    return out;
}

}  // namespace sotc
