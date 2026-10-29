#include "ctor_slot_365_747870.hpp"

namespace sotc {

Slot365Result init_slot365_747870(Slot365Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot365Result out{};
    out.var = init_bool_var_site(objects.var, kSlot365Name, kSlot365FnTag, concmd, 0, 0);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
