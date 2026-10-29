#include "ctor_slot_393_66cc00.hpp"

namespace sotc {

Slot393Result init_slot393_66cc00(Slot393Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot393Result out{};
    out.var = init_bool_var_site(objects.var, kSlot393VarName, kSlot393FnTag, concmd, 0, 0);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
