#include "ctor_slot_479_31b660.hpp"

namespace sotc {

Slot479Result init_slot479_31b660(Slot479Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot479Result out{};
    out.var = init_bool_var_site(objects.var, kSlot479Name, kSlot479FnTag, concmd, kSlot479Value, kSlot479Default);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
