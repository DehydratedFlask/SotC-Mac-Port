#include "ctor_slot_459_43f460.hpp"

namespace sotc {

Slot459Result init_slot459_43f460(Slot459Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot459Result out{};
    out.var = init_bool_var_site(objects.var, kSlot459Name, kSlot459FnTag, concmd, 0, 0);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
