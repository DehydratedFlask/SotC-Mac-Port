#include "ctor_slot_376_6e4930.hpp"

namespace sotc {

Slot376Result init_slot376_6e4930(Slot376Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot376Result out{};
    out.var = init_bool_var_site(objects.var, kSlot376Name, kSlot376FnTag, concmd, 0, 0);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
