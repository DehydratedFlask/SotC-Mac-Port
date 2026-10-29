#include "ctor_slot_458_45ace0.hpp"

namespace sotc {

Slot458Result init_slot458_45ace0(Slot458Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot458Result out{};
    out.var = init_bool_var_site(objects.var, kSlot458Name, kSlot458FnTag, concmd, 0, 0);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
