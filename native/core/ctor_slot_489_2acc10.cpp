#include "ctor_slot_489_2acc10.hpp"

namespace sotc {

Slot489Result init_slot489_2acc10(Slot489Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot489Result out{};
    const SharedStr name = lane_m4::borrowed_view_of(kSlot489Name, sizeof kSlot489Name - 1);
    const SharedStr description = lane_m4::borrowed_view_of("", 0);   // rodata NUL byte 0xfcaec4
    out.var = lane_m4::init_int_var_site(objects.var, name, description, kSlot489ArgFnTag, concmd, kSlot489Value,
                                         kSlot489Default);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
