#include "ctor_slot_490_296d90.hpp"

namespace sotc {

Slot490Result init_slot490_296d90(Slot490Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot490Result out{};
    out.var = init_bool_var_site(objects.var, kSlot490Name, kSlot490FnTag, concmd, kSlot490Value, kSlot490Default);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
