#include "ctor_slot_475_33cc60.hpp"

namespace sotc {

Slot475Result init_slot475_33cc60(Slot475Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot475Result out{};
    const SharedStr name = lane_m4::borrowed_view_of(kSlot475Name, sizeof kSlot475Name - 1);
    const SharedStr description = lane_m4::borrowed_view_of(kSlot475Description, sizeof kSlot475Description - 1);
    out.var = lane_m4::init_bool_var_late_release(objects.var, name, description, kSlot475FnTag, concmd, kSlot475Value, kSlot475Default);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
