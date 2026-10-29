#include "ctor_slot_461_419480.hpp"

namespace sotc {

Slot461Result init_slot461_419480(Slot461Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot461Result out{};
    out.var = init_bool_var_site(objects.var, kSlot461Name, kSlot461FnTag, concmd, 1, 1);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
