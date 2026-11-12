#include "ctor_slot_503_247f00.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot503ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}




// Console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], two f590, __cxa_atexit(0x4b0)); the handler is an identity tag, never called.
[[maybe_unused]] bool cmd_var(ConCmdCommand& command, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot503CmdDesc[n];
    const FConCmdCommandSpec spec{kSlot503CmdNames[n], kSlot503MenuStrings[d], kSlot503MenuStringLengths[d], kSlot503CmdHandler[n]};
    const FConCmdBuildResult built = init_fconcmd_object(command, spec, concmd);
    return built.ok && built.vtable_register_slot_ok && built.views_inert && built.atexit_recorded && !built.handler_invoked;
}


[[maybe_unused]] bool ufloat_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot503UFloatDesc[n];
    const char* const desc = d < 0 ? "" : kSlot503MenuStrings[d];
    const std::size_t desc_length = d < 0 ? 0 : kSlot503MenuStringLengths[d];
    const HelperFloatSiteSpec spec{kSlot503UFloatNames[n], kSlot503UFloatNameLengths[n], desc, desc_length, kSlot503VarFlags, kSlot503UFloatFnTag, kSlot503UFloatValueBits[n], kSlot503UFloatEarly[n] != 0};
    return helper_init_unbounded_float_var_site(var, spec, concmd).ok;
}




[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot503BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot503BoolNames[n], kSlot503BoolNameLengths[n], d < 0 ? "" : kSlot503MenuStrings[d], d < 0 ? 0 : kSlot503MenuStringLengths[d],
                                          kSlot503BoolFnTag, concmd, kSlot503BoolValues[n], kSlot503BoolValues[n], kSlot503BoolEarly[n] != 0).ok;
}


// The 0x14 byte null-name record: [obj] = 0 ; strncpy(obj+4, "NULL", 15) (ISO C strncpy, helper_f8f630_strncpy) ; byte +0x13 = 0. No atexit record.
bool null_name_cell(NullNameCell247f47& cell) noexcept {
    store_null_name_cell_247f47(cell);
    return true;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot503Result init_slot503_247f00(Slot503Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot503Result out{};

    if (!null_name_cell(objects.r00)) return out;
    ++out.rows_done;   // row 00 ncell
    if (!bool_var(objects.r01, 0, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!ufloat_var(objects.r02, 0, concmd)) return out;
    ++out.rows_done;   // row 02 ufloat
    if (!bool_var(objects.r03, 1, concmd)) return out;
    ++out.rows_done;   // row 03 bool
    if (!cmd_var(objects.r04, 0, concmd)) return out;
    ++out.rows_done;   // row 04 cmd
    if (!cmd_var(objects.r05, 1, concmd)) return out;
    ++out.rows_done;   // row 05 cmd
    if (!cmd_var(objects.r06, 2, concmd)) return out;
    ++out.rows_done;   // row 06 cmd
    if (!cmd_var(objects.r07, 3, concmd)) return out;
    ++out.rows_done;   // row 07 cmd
    if (!cmd_var(objects.r08, 4, concmd)) return out;
    ++out.rows_done;   // row 08 cmd
    if (!cmd_var(objects.r09, 5, concmd)) return out;
    ++out.rows_done;   // row 09 cmd
    if (!cmd_var(objects.r10, 6, concmd)) return out;
    ++out.rows_done;   // row 10 cmd
    if (!cmd_var(objects.r11, 7, concmd)) return out;
    ++out.rows_done;   // row 11 cmd
    if (!cmd_var(objects.r12, 8, concmd)) return out;
    ++out.rows_done;   // row 12 cmd
    if (!bool_var(objects.r13, 2, concmd)) return out;
    ++out.rows_done;   // row 13 bool
    if (!family_toggle_item_26ea0(objects.r14, kSlot503MenuStrings[13], kSlot503MenuStringLengths[13], kSlot503MenuStrings[14], kSlot503MenuStringLengths[14], target_of(objects.r13), kSlot503ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 14 toge
    out.ok = true;
    return out;
}


}  // namespace sotc
