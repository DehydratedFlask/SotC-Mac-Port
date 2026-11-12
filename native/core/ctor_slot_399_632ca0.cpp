#include "ctor_slot_399_632ca0.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot399ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}

// The 0x27050 slider: ctor(item, label, text, target, 0x30, lo, hi) (explicit text, any target object), 0x268b0, f590(text), f590(label), __cxa_atexit(0x272d0, item).
[[maybe_unused]] bool slider50_item(DebugMenuItem& item, const char* label, std::size_t label_length, const char* text, std::size_t text_length, const void* target,
                                    std::uint32_t lo_bits, std::uint32_t hi_bits, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    float lo = 0.0f;
    float hi = 0.0f;
    std::memcpy(&lo, &lo_bits, sizeof lo);
    std::memcpy(&hi, &hi_bits, sizeof hi);
    const SharedStr label_view = family_view(label, label_length);
    const SharedStr text_view = family_view(text, text_length);
    if (!init_debug_menu_item_27050(item, label_view, text_view, target, kSlot399ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(text_view) || !release_inert_view_f590(label_view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}



[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot399FloatNames[n], "", true, kSlot399FloatLoBits[n], kSlot399FloatHiBits[n], kSlot399FloatValueBits[n], kSlot399FloatFnTag, kSlot399VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}



[[maybe_unused]] bool lint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return helper_init_local_int_var_site(var, kSlot399LocalIntSites[n], empty_string_f730(), concmd).ok;
}

[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot399BoolNames[n], kSlot399BoolNameLengths[n], "", 0, kSlot399BoolFnTag, concmd, kSlot399BoolValues[n],
                                          kSlot399BoolValues[n]).ok;
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

Slot399Result init_slot399_632ca0(Slot399Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot399Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!bool_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!bool_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 bool
    if (!float_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 float
    if (!float_var(objects.r04, 1, concmd)) return out;
    ++out.rows_done;   // row 04 float
    if (!bool_var(objects.r05, 3, concmd)) return out;
    ++out.rows_done;   // row 05 bool
    if (!float_var(objects.r06, 2, concmd)) return out;
    ++out.rows_done;   // row 06 float
    if (!float_var(objects.r07, 3, concmd)) return out;
    ++out.rows_done;   // row 07 float
    if (!bool_var(objects.r08, 4, concmd)) return out;
    ++out.rows_done;   // row 08 bool
    if (!lint_var(objects.r09, 0, concmd)) return out;
    ++out.rows_done;   // row 09 lint
    if (!lint_var(objects.r10, 1, concmd)) return out;
    ++out.rows_done;   // row 10 lint
    if (!family_toggle_item_26ea0(objects.r11, kSlot399MenuStrings[0], kSlot399MenuStringLengths[0], kSlot399MenuStrings[1], kSlot399MenuStringLengths[1], local_int_var_menu_target(objects.r09), kSlot399ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 11 toge
    if (!family_toggle_item_26ea0(objects.r12, kSlot399MenuStrings[0], kSlot399MenuStringLengths[0], kSlot399MenuStrings[2], kSlot399MenuStringLengths[2], local_int_var_menu_target(objects.r10), kSlot399ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 12 toge
    if (!family_toggle_item_26ea0(objects.r13, kSlot399MenuStrings[3], kSlot399MenuStringLengths[3], kSlot399MenuStrings[4], kSlot399MenuStringLengths[4], target_of(objects.r01), kSlot399ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 13 toge
    if (!family_toggle_item_26ea0(objects.r14, kSlot399MenuStrings[3], kSlot399MenuStringLengths[3], kSlot399MenuStrings[5], kSlot399MenuStringLengths[5], target_of(objects.r02), kSlot399ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 14 toge
    if (!slider50_item(objects.r15, kSlot399MenuStrings[3], kSlot399MenuStringLengths[3], kSlot399MenuStrings[6], kSlot399MenuStringLengths[6], &objects.r03, kSlot399SliderLoBits[0], kSlot399SliderHiBits[0], concmd, menu)) return out;
    ++out.rows_done;   // row 15 slider50
    if (!slider50_item(objects.r16, kSlot399MenuStrings[7], kSlot399MenuStringLengths[7], kSlot399MenuStrings[8], kSlot399MenuStringLengths[8], &objects.r06, kSlot399SliderLoBits[1], kSlot399SliderHiBits[1], concmd, menu)) return out;
    ++out.rows_done;   // row 16 slider50
    if (!family_toggle_item_26ea0(objects.r17, kSlot399MenuStrings[7], kSlot399MenuStringLengths[7], kSlot399MenuStrings[9], kSlot399MenuStringLengths[9], target_of(objects.r05), kSlot399ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 17 toge
    if (!slider50_item(objects.r18, kSlot399MenuStrings[7], kSlot399MenuStringLengths[7], kSlot399MenuStrings[10], kSlot399MenuStringLengths[10], &objects.r04, kSlot399SliderLoBits[2], kSlot399SliderHiBits[2], concmd, menu)) return out;
    ++out.rows_done;   // row 18 slider50
    if (!slider50_item(objects.r19, kSlot399MenuStrings[7], kSlot399MenuStringLengths[7], kSlot399MenuStrings[11], kSlot399MenuStringLengths[11], &objects.r07, kSlot399SliderLoBits[3], kSlot399SliderHiBits[3], concmd, menu)) return out;
    ++out.rows_done;   // row 19 slider50
    if (!family_toggle_item_26ea0(objects.r20, kSlot399MenuStrings[12], kSlot399MenuStringLengths[12], kSlot399MenuStrings[13], kSlot399MenuStringLengths[13], target_of(objects.r08), kSlot399ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 20 toge
    if (!bool_var(objects.r21, 5, concmd)) return out;
    ++out.rows_done;   // row 21 bool
    if (!name_cell(objects.r22, kSlot399MenuStrings[14])) return out;
    ++out.rows_done;   // row 22 cell
    if (!name_cell(objects.r23, kSlot399MenuStrings[15])) return out;
    ++out.rows_done;   // row 23 cell
    if (!name_cell(objects.r24, kSlot399MenuStrings[16])) return out;
    ++out.rows_done;   // row 24 cell
    if (!name_cell(objects.r25, kSlot399MenuStrings[17])) return out;
    ++out.rows_done;   // row 25 cell
    if (!name_cell(objects.r26, kSlot399MenuStrings[18])) return out;
    ++out.rows_done;   // row 26 cell
    if (!name_cell(objects.r27, kSlot399MenuStrings[19])) return out;
    ++out.rows_done;   // row 27 cell
    if (!name_cell(objects.r28, kSlot399MenuStrings[20])) return out;
    ++out.rows_done;   // row 28 cell
    if (!name_cell(objects.r29, kSlot399MenuStrings[21])) return out;
    ++out.rows_done;   // row 29 cell
    if (!name_cell(objects.r30, kSlot399MenuStrings[22])) return out;
    ++out.rows_done;   // row 30 cell
    out.ok = true;
    return out;
}

}  // namespace sotc
