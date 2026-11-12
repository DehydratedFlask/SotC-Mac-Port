#include "ctor_slot_566_25de0.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot566ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}


[[maybe_unused]] bool uint_var(ConCmdRangedVar& var, std::size_t n, std::uint32_t* cell, ConCmdRegistry& concmd) noexcept {
    if (kSlot566UIntTwin[n] != 0) {
        const HelperIntTwinSiteSpec twin{kSlot566UIntNames[n], kSlot566UIntNameLengths[n], "", 0, kSlot566VarFlags, kSlot566UIntValue[n], kSlot566UIntValue[n], kSlot566UIntTwinFinalVptr, kSlot566UIntTwinDtorRva, kSlot566UIntEarly[n] != 0, false, false};
        return helper_init_unbounded_int_twin_var_site(var, UnboundedIntTwinVariant::b_25ab0, twin, cell, concmd).ok;
    }
    const HelperIntSiteSpec spec{kSlot566UIntNames[n], kSlot566UIntNameLengths[n], "", 0, 0, 0, kSlot566VarFlags, kSlot566UIntFnTag, kSlot566UIntValue[n], kSlot566UIntValue[n], kSlot566UIntEarly[n] != 0};
    return helper_init_unbounded_int_var_site(var, spec, concmd).ok;
}


// String variable (0x257a0 -> 0x258f0): borrowed name and initial views, then f590(init), f590(name), __cxa_atexit(0x23c90, var).
[[maybe_unused]] bool str_var(ConCmdStringVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const SharedStr name = family_view(kSlot566StrNames[n], kSlot566StrNameLengths[n]);
    const std::int32_t d = kSlot566StrInit[n];
    const SharedStr init = family_view(kSlot566MenuStrings[d], kSlot566MenuStringLengths[d]);
    if (!init_string_var_257a0(var, name, init, concmd).ok) return false;
    if (!release_inert_view_f590(init) || !release_inert_view_f590(name)) return false;
    return concmd.atexit_log.record(kStringVarDtorRva, &var);
}







[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot566BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot566BoolNames[n], kSlot566BoolNameLengths[n], d < 0 ? "" : kSlot566MenuStrings[d], d < 0 ? 0 : kSlot566MenuStringLengths[d],
                                          kSlot566BoolFnTag, concmd, kSlot566BoolValues[n], kSlot566BoolValues[n], kSlot566BoolEarly[n] != 0).ok;
}


[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot566Result init_slot566_25de0(Slot566Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot566Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!str_var(objects.r01, 0, concmd)) return out;
    ++out.rows_done;   // row 01 str
    if (!str_var(objects.r02, 1, concmd)) return out;
    ++out.rows_done;   // row 02 str
    if (!bool_var(objects.r03, 1, concmd)) return out;
    ++out.rows_done;   // row 03 bool
    if (!uint_var(objects.r04, 0, &objects.cell04, concmd)) return out;
    ++out.rows_done;   // row 04 uint
    if (!family_toggle_item_26ea0(objects.r05, kSlot566MenuStrings[2], kSlot566MenuStringLengths[2], kSlot566MenuStrings[3], kSlot566MenuStringLengths[3], inline_enum_menu_target(objects.r04, kSlot566UIntTwinFinalVptr, kSlot566UIntTwinSlot3Result, kSlot566UIntTwinSlot6Result), kSlot566ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 05 toge
    out.ok = true;
    return out;
}


}  // namespace sotc
