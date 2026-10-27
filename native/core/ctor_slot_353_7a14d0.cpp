#include "ctor_slot_353_7a14d0.hpp"

namespace sotc {

Slot353Result init_slot353_7a14d0(Slot353Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot353Result out{};
    out.flag = init_bool_var_site(objects.flag, kSlot353BoolName, kSlot353BoolFnTag, concmd, 1, 1);
    if (!out.flag.ok) return out;
    out.offset = init_float_var_static(objects.offset, kSlot353FloatSpec, concmd);
    out.ok = out.flag.ok && out.offset.ok;
    return out;
}

}  // namespace sotc
