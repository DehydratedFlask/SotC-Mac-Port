#include "ctor_slot_502_249270.hpp"

namespace sotc {

Slot502Result init_slot502_249270(Slot502Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot502Result out{};
    out.var = init_bool_var_site(objects.var, kSlot502Name, kSlot502FnTag, concmd, 0, 0);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
