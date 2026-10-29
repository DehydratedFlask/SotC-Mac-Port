#include "ctor_slot_482_2fb2a0.hpp"

namespace sotc {

Slot482Result init_slot482_2fb2a0(Slot482Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot482Result out{};
    out.var = init_bool_var_site(objects.var, kSlot482Name, kSlot482FnTag, concmd, kSlot482Value, kSlot482Default);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
