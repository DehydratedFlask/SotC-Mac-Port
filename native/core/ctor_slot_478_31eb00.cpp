#include "ctor_slot_478_31eb00.hpp"

namespace sotc {

Slot478Result init_slot478_31eb00(Slot478Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot478Result out{};
    const SharedStr name = lane_m4::borrowed_view_of(kSlot478Name, sizeof kSlot478Name - 1);
    const SharedStr description = lane_m4::borrowed_view_of(kSlot478Description, sizeof kSlot478Description - 1);
    out.var = lane_m4::init_bool_var_late_release(objects.var, name, description, kSlot478FnTag, concmd, kSlot478Value, kSlot478Default);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
