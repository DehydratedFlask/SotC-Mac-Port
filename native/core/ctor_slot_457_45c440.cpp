#include "ctor_slot_457_45c440.hpp"

namespace sotc {

Slot457Result init_slot457_45c440(Slot457Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot457Result out{};
    out.var = init_bool_var_site(objects.var, kSlot457Name, kSlot457FnTag, concmd, 0, 0);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
