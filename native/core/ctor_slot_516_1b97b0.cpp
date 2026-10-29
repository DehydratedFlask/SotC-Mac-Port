#include "ctor_slot_516_1b97b0.hpp"

#include "l2_slot_common.hpp"

namespace sotc {

Slot516Result init_slot516_1b97b0(Slot516Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot516Result out{};
    const SharedStr name = l2::literal_view(kSlot516Name);
    const SharedStr init = l2::literal_view("");
    out.var = init_string_var_257a0(objects.var, name, init, concmd);                    // 0x1b981a
    if (!out.var.ok) { out.blocked = Slot516Stage::string_var; return out; }
    if (!release_inert_view_f590(init) || !release_inert_view_f590(name)) {              // 0x1b9822, 0x1b982a
        out.blocked = Slot516Stage::view_release;
        return out;
    }
    if (!concmd.atexit_log.record(kStringVarDtorRva, &objects.var)) {                    // 0x1b9840
        out.blocked = Slot516Stage::atexit;
        return out;
    }
    out.ok = true;
    return out;
}

}  // namespace sotc
