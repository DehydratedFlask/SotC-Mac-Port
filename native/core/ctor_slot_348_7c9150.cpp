#include "ctor_slot_348_7c9150.hpp"

namespace sotc {

Slot348Result init_slot348_7c9150(Slot348Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot348Result out{};
    out.var = init_bool_var_site(objects.var, kSlot348Name, kSlot348FnTag, concmd, 1, 1);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
