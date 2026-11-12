#include "ctor_slot_501_24db80.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot501ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}


[[maybe_unused]] bool uint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const HelperIntSiteSpec spec{kSlot501UIntNames[n], kSlot501UIntNameLengths[n], "", 0, 0, 0, kSlot501VarFlags, kSlot501UIntFnTag, kSlot501UIntValue[n], kSlot501UIntValue[n], kSlot501UIntEarly[n] != 0};
    return helper_init_unbounded_int_var_site(var, spec, concmd).ok;
}


// String variable (0x257a0 -> 0x258f0): borrowed name and initial views, then f590(init), f590(name), __cxa_atexit(0x23c90, var).
[[maybe_unused]] bool str_var(ConCmdStringVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const SharedStr name = family_view(kSlot501StrNames[n], kSlot501StrNameLengths[n]);
    const std::int32_t d = kSlot501StrInit[n];
    const SharedStr init = d < 0 ? empty_string_f730() : family_view(kSlot501MenuStrings[d], kSlot501MenuStringLengths[d]);   // d < 0: the static empty string returned by 0xf730 (never released)
    if (!init_string_var_257a0(var, name, init, concmd).ok) return false;
    if ((d >= 0 && !release_inert_view_f590(init)) || !release_inert_view_f590(name)) return false;
    return concmd.atexit_log.record(kStringVarDtorRva, &var);
}



[[maybe_unused]] bool ufloat_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot501UFloatDesc[n];
    const char* const desc = d < 0 ? "" : kSlot501MenuStrings[d];
    const std::size_t desc_length = d < 0 ? 0 : kSlot501MenuStringLengths[d];
    const HelperFloatSiteSpec spec{kSlot501UFloatNames[n], kSlot501UFloatNameLengths[n], desc, desc_length, kSlot501VarFlags, kSlot501UFloatFnTag, kSlot501UFloatValueBits[n], kSlot501UFloatEarly[n] != 0};
    return helper_init_unbounded_float_var_site(var, spec, concmd).ok;
}




[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot501BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot501BoolNames[n], kSlot501BoolNameLengths[n], d < 0 ? "" : kSlot501MenuStrings[d], d < 0 ? 0 : kSlot501MenuStringLengths[d],
                                          kSlot501BoolFnTag, concmd, kSlot501BoolValues[n], kSlot501BoolValues[n], kSlot501BoolEarly[n] != 0).ok;
}


[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot501Result init_slot501_24db80(Slot501Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot501Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!bool_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!bool_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 bool
    if (!ufloat_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 ufloat
    if (!bool_var(objects.r04, 3, concmd)) return out;
    ++out.rows_done;   // row 04 bool
    if (!str_var(objects.r05, 0, concmd)) return out;
    ++out.rows_done;   // row 05 str
    if (!uint_var(objects.r06, 0, concmd)) return out;
    ++out.rows_done;   // row 06 uint
    if (!bool_var(objects.r07, 4, concmd)) return out;
    ++out.rows_done;   // row 07 bool
    if (!family_group_item_26c10(objects.r08, kSlot501MenuStrings[0], kSlot501MenuStringLengths[0], kSlot501MenuStrings[1], kSlot501MenuStringLengths[1], 0x20, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 08 group
    if (!family_label_item_26800(objects.r09, kSlot501MenuStrings[1], kSlot501MenuStringLengths[1], 0x4, objects.separator_alloc, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 09 sep
    if (!family_toggle_item_26ea0(objects.r10, kSlot501MenuStrings[1], kSlot501MenuStringLengths[1], kSlot501MenuStrings[2], kSlot501MenuStringLengths[2], target_of(objects.r07), kSlot501ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 10 toge
    if (!family_toggle_item_26ea0(objects.r11, kSlot501MenuStrings[1], kSlot501MenuStringLengths[1], kSlot501MenuStrings[3], kSlot501MenuStringLengths[3], target_of(objects.r01), kSlot501ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 11 toge
    if (!family_toggle_item_26ea0(objects.r12, kSlot501MenuStrings[1], kSlot501MenuStringLengths[1], kSlot501MenuStrings[4], kSlot501MenuStringLengths[4], target_of(objects.r03), kSlot501ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 12 toge
    if (!family_toggle_item_26ea0(objects.r13, kSlot501MenuStrings[1], kSlot501MenuStringLengths[1], kSlot501MenuStrings[5], kSlot501MenuStringLengths[5], target_of(objects.r04), kSlot501ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 13 toge
    if (!family_toggle_item_26ea0(objects.r14, kSlot501MenuStrings[1], kSlot501MenuStringLengths[1], kSlot501MenuStrings[6], kSlot501MenuStringLengths[6], target_of(objects.r02), kSlot501ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 14 toge
    if (!family_toggle_item_26ea0(objects.r15, kSlot501MenuStrings[1], kSlot501MenuStringLengths[1], kSlot501MenuStrings[7], kSlot501MenuStringLengths[7], target_of(objects.r00), kSlot501ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 15 toge
    if (!family_toggle_item_26ea0(objects.r16, kSlot501MenuStrings[1], kSlot501MenuStringLengths[1], kSlot501MenuStrings[8], kSlot501MenuStringLengths[8], string_var_menu_target(objects.r05), kSlot501ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 16 toge
    if (!family_toggle_item_26ea0(objects.r17, kSlot501MenuStrings[1], kSlot501MenuStringLengths[1], kSlot501MenuStrings[9], kSlot501MenuStringLengths[9], unbounded_int_menu_target(objects.r06), kSlot501ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 17 toge
    out.ok = true;
    return out;
}


}  // namespace sotc
