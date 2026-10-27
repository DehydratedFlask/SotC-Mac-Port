#include "ctor_slot_347_7c93a0.hpp"

namespace sotc {

Slot347Result init_slot347_7c93a0(Slot347Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot347Result out{};
    out.var = init_float_var_static(objects.var, kSlot347Spec, concmd);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
