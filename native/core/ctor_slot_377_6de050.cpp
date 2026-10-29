#include "ctor_slot_377_6de050.hpp"

namespace sotc {

Slot377Result init_slot377_6de050(Slot377Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot377Result out{};
    out.var = init_bool_var_site(objects.var, kSlot377Name, kSlot377FnTag, concmd, 1, 1);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
