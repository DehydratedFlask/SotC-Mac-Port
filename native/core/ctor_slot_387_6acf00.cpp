#include "ctor_slot_387_6acf00.hpp"

namespace sotc {

Slot387Result init_slot387_6acf00(Slot387Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot387Result out{};
    out.var = init_bool_var_site(objects.var, kSlot387Name, kSlot387FnTag, concmd, 1, 1);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
