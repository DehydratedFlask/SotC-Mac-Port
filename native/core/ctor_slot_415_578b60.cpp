#include "ctor_slot_415_578b60.hpp"

namespace sotc {

Slot415Result init_slot415_578b60(Slot415Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot415Result out{};
    out.var = lane_m2::init_unbounded_int_var_site(objects.var, kSlot415Spec, concmd);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
