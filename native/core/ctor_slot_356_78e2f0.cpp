#include "ctor_slot_356_78e2f0.hpp"

namespace sotc {

Slot356Result init_slot356_78e2f0(Slot356Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot356Result out{};
    out.var = init_bool_var_site(objects.var, kSlot356Name, kSlot356FnTag, concmd, 1, 1);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
