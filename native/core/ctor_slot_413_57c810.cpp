#include "ctor_slot_413_57c810.hpp"

namespace sotc {

Slot413Result init_slot413_57c810(Slot413Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot413Result out{};
    out.var = lane_m2::init_float_var_site(objects.var, kSlot413Spec, concmd);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
