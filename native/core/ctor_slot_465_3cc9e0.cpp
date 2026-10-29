#include "ctor_slot_465_3cc9e0.hpp"

namespace sotc {

Slot465Result init_slot465_3cc9e0(Slot465Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot465Result out{};
    out.var = init_bool_var_site(objects.var, kSlot465Name, kSlot465FnTag, concmd, kSlot465Value, kSlot465Default);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
