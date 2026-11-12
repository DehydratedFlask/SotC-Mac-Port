#include "ctor_slot_342_7d8e10.hpp"

namespace sotc {

Slot342Result init_slot342_7d8e10(Slot342Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot342Result out{};
    // 0x7d8e3d..0x7d8f2d: bool variable, empty description, caller tag 0x15ce3d0, value = default 1, atexit 0x20b0.
    if (!lane_m6::init_bool_var_literal(objects.bvar, kSlot342BoolName, kSlot342BoolNameLength, "", 0, kSlot342BoolFnTag, concmd, kSlot342BoolValue,
                                        kSlot342BoolValue).ok) {
        return out;
    }
    ++out.rows_done;
    // 0x7d8f32..0x7d9112: two unbounded float variables, empty descriptions, early description release, atexit 0x20f0 each.
    for (std::size_t n = 0; n < kSlot342FloatCount; ++n) {
        const HelperFloatSiteSpec spec{kSlot342FloatNames[n], kSlot342FloatNameLengths[n], "", 0, kSlot342VarFlags, kSlot342FloatFnTag,
                                       kSlot342FloatValueBits, true};
        out.fvar[n] = helper_init_unbounded_float_var_site(objects.fvar[n], spec, concmd);
        out.refusal = out.fvar[n].refusal;
        if (!out.fvar[n].ok) return out;
        ++out.rows_done;
    }
    // 0x7d9112..0x7d9129: 0x23fd00(cell, NULL) twice; no atexit record.
    for (std::size_t n = 0; n < kSlot342NameCellCount; ++n) init_name_cell_23fd00(objects.cells[n], nullptr);
    out.rows_done += kSlot342NameCellCount;
    // 0x7d912e..0x7d9210: the console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], atexit 0x4b0).
    const FConCmdCommandSpec cmd{kSlot342CommandName, kSlot342CommandDescription, kSlot342CommandDescriptionLength, kSlot342HandlerTag};
    out.command = init_fconcmd_object(objects.command, cmd, concmd);
    if (!out.command.ok) return out;
    ++out.rows_done;
    out.ok = true;
    return out;
}

}  // namespace sotc
