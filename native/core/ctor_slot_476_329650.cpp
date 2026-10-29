#include "ctor_slot_476_329650.hpp"

namespace sotc {

Slot476Result init_slot476_329650(Slot476Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot476Result out{};
    out.var = init_bool_var_site(objects.var, kSlot476Name, kSlot476FnTag, concmd, kSlot476Value, kSlot476Default);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
