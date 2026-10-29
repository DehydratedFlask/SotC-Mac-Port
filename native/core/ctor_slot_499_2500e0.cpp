#include "ctor_slot_499_2500e0.hpp"

namespace sotc {

Slot499Result init_slot499_2500e0(Slot499Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot499Result out{};
    out.var = init_bool_var_site(objects.var, kSlot499Name, kSlot499FnTag, concmd, 0, 0);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
