#include "ctor_slot_480_30daf0.hpp"

namespace sotc {

Slot480Result init_slot480_30daf0(Slot480Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot480Result out{};
    // Three bounded float variables (0xc300, empty description, caller tag 0x15a66b8, value = default), atexit 0x20f0 each.
    for (std::size_t n = 0; n < kSlot480FloatCount; ++n) {
        const FloatVarSpec spec{kSlot480FloatNames[n], "", true, kSlot480FloatLoBits, kSlot480FloatHiBits, kSlot480FloatValueBits[n], kSlot480FloatFnTag,
                                kSlot480VarFlags};
        if (!init_float_var_static(objects.fvar[n], spec, concmd).ok) return out;
        ++out.rows_done;
    }
    // Two bool variables (empty description, caller tag 0x15a66d8, value = default 1), atexit 0x20b0 each.
    for (std::size_t n = 0; n < kSlot480BoolCount; ++n) {
        if (!lane_m6::init_bool_var_literal(objects.bvar[n], kSlot480BoolNames[n], kSlot480BoolNameLengths[n], "", 0, kSlot480BoolFnTag, concmd,
                                            kSlot480BoolValue, kSlot480BoolValue).ok) {
            return out;
        }
        ++out.rows_done;
    }
    // Two console commands (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], atexit 0x4b0 each).
    for (std::size_t n = 0; n < kSlot480CommandCount; ++n) {
        const FConCmdCommandSpec cmd{kSlot480CommandNames[n], kSlot480CommandDescriptions[n], kSlot480CommandDescriptionLengths[n], kSlot480HandlerTags[n]};
        out.command[n] = init_fconcmd_object(objects.command[n], cmd, concmd);
        if (!out.command[n].ok) return out;
        ++out.rows_done;
    }
    out.ok = true;
    return out;
}

}  // namespace sotc
