#include "ctor_slot_381_6cd5b0.hpp"

#include <cstring>

namespace sotc {

namespace {

// The 0x27120 slider: ctor(item, label, variable, 0x30, lo, hi) (text = the variable's own name view), 0x268b0, 0xf590(label), __cxa_atexit(0x272d0, item).
[[maybe_unused]] bool slider_item(DebugMenuItem& item, const char* label, std::size_t label_length, const ConCmdRangedVar& var, std::uint32_t lo_bits,
                 std::uint32_t hi_bits, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    float lo = 0.0f;
    float hi = 0.0f;
    std::memcpy(&lo, &lo_bits, sizeof lo);
    std::memcpy(&hi, &hi_bits, sizeof hi);
    const SharedStr view = family_view(label, label_length);
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot381ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}



// Console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], two f590, __cxa_atexit(0x4b0)); the handler is an identity tag, never called.
[[maybe_unused]] bool cmd_var(ConCmdCommand& command, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot381CmdDesc[n];
    const FConCmdCommandSpec spec{kSlot381CmdNames[n], kSlot381MenuStrings[d], kSlot381MenuStringLengths[d], kSlot381CmdHandler[n]};
    const FConCmdBuildResult built = init_fconcmd_object(command, spec, concmd);
    return built.ok && built.vtable_register_slot_ok && built.views_inert && built.atexit_recorded && !built.handler_invoked;
}

[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot381FloatNames[n], "", true, kSlot381FloatLoBits[n], kSlot381FloatHiBits[n], kSlot381FloatValueBits[n], kSlot381FloatFnTag, kSlot381VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}

[[maybe_unused]] bool ufloat_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot381UFloatDesc[n];
    const char* const desc = d < 0 ? "" : kSlot381MenuStrings[d];
    const std::size_t desc_length = d < 0 ? 0 : kSlot381MenuStringLengths[d];
    const HelperFloatSiteSpec spec{kSlot381UFloatNames[n], kSlot381UFloatNameLengths[n], desc, desc_length, kSlot381VarFlags, kSlot381UFloatFnTag, kSlot381UFloatValueBits[n], kSlot381UFloatEarly[n] != 0};
    return helper_init_unbounded_float_var_site(var, spec, concmd).ok;
}




// `[cell] = name; cell->hash = 0x52a990(name)`; the four bytes at +0x0c are never written.
bool name_cell(NameHashCell& cell, const char* name) noexcept {
    cell.name = name;
    cell.hash = fnv_cstr_52a990(name);
    return true;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot381Result init_slot381_6cd5b0(Slot381Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot381Result out{};

    if (!float_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 float
    if (!float_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 float
    if (!float_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 float
    if (!float_var(objects.r03, 3, concmd)) return out;
    ++out.rows_done;   // row 03 float
    if (!float_var(objects.r04, 4, concmd)) return out;
    ++out.rows_done;   // row 04 float
    if (!ufloat_var(objects.r05, 0, concmd)) return out;
    ++out.rows_done;   // row 05 ufloat
    if (!ufloat_var(objects.r06, 1, concmd)) return out;
    ++out.rows_done;   // row 06 ufloat
    if (!ufloat_var(objects.r07, 2, concmd)) return out;
    ++out.rows_done;   // row 07 ufloat
    if (!cmd_var(objects.r08, 0, concmd)) return out;
    ++out.rows_done;   // row 08 cmd
    if (!cmd_var(objects.r09, 1, concmd)) return out;
    ++out.rows_done;   // row 09 cmd
    if (!cmd_var(objects.r10, 2, concmd)) return out;
    ++out.rows_done;   // row 10 cmd
    if (!name_cell(objects.r11, kSlot381MenuStrings[4])) return out;
    ++out.rows_done;   // row 11 cell
    if (!name_cell(objects.r12, kSlot381MenuStrings[5])) return out;
    ++out.rows_done;   // row 12 cell
    if (!name_cell(objects.r13, kSlot381MenuStrings[6])) return out;
    ++out.rows_done;   // row 13 cell
    if (!name_cell(objects.r14, kSlot381MenuStrings[7])) return out;
    ++out.rows_done;   // row 14 cell
    if (!name_cell(objects.r15, kSlot381MenuStrings[8])) return out;
    ++out.rows_done;   // row 15 cell
    if (!name_cell(objects.r16, kSlot381MenuStrings[9])) return out;
    ++out.rows_done;   // row 16 cell
    out.ok = true;
    return out;
}

}  // namespace sotc
