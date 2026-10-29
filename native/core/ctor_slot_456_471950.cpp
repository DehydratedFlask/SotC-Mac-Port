#include "ctor_slot_456_471950.hpp"

namespace sotc {

Slot456Result init_slot456_471950(Slot456Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot456Result out{};
    out.var = init_bool_var_site(objects.var, kSlot456Name, kSlot456FnTag, concmd, 1, 1);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
