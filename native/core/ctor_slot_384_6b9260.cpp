#include "ctor_slot_384_6b9260.hpp"

namespace sotc {

Slot384Result init_slot384_6b9260(Slot384Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot384Result out{};
    out.var = init_bool_var_site(objects.var, kSlot384Name, kSlot384FnTag, concmd, 0, 0);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
