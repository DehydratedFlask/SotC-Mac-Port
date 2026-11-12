#include "ctor_slot_375_6f1c20.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot375ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}

[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot375FloatNames[n], "", true, kSlot375FloatLoBits[n], kSlot375FloatHiBits[n], kSlot375FloatValueBits[n], kSlot375FloatFnTag, kSlot375VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}


[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot375BoolNames[n], kSlot375BoolNameLengths[n], "", 0, kSlot375BoolFnTag, concmd, kSlot375BoolValues[n],
                                          kSlot375BoolValues[n]).ok;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot375Result init_slot375_6f1c20(Slot375Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot375Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!bool_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!float_var(objects.r02, 0, concmd)) return out;
    ++out.rows_done;   // row 02 float
    if (!float_var(objects.r03, 1, concmd)) return out;
    ++out.rows_done;   // row 03 float
    if (!float_var(objects.r04, 2, concmd)) return out;
    ++out.rows_done;   // row 04 float
    if (!float_var(objects.r05, 3, concmd)) return out;
    ++out.rows_done;   // row 05 float
    if (!float_var(objects.r06, 4, concmd)) return out;
    ++out.rows_done;   // row 06 float
    if (!float_var(objects.r07, 5, concmd)) return out;
    ++out.rows_done;   // row 07 float
    if (!float_var(objects.r08, 6, concmd)) return out;
    ++out.rows_done;   // row 08 float
    if (!bool_var(objects.r09, 2, concmd)) return out;
    ++out.rows_done;   // row 09 bool
    if (!bool_var(objects.r10, 3, concmd)) return out;
    ++out.rows_done;   // row 10 bool
    if (!bool_var(objects.r11, 4, concmd)) return out;
    ++out.rows_done;   // row 11 bool
    if (!bool_var(objects.r12, 5, concmd)) return out;
    ++out.rows_done;   // row 12 bool
    if (!bool_var(objects.r13, 6, concmd)) return out;
    ++out.rows_done;   // row 13 bool
    if (!bool_var(objects.r14, 7, concmd)) return out;
    ++out.rows_done;   // row 14 bool
    if (!bool_var(objects.r15, 8, concmd)) return out;
    ++out.rows_done;   // row 15 bool
    if (!float_var(objects.r16, 7, concmd)) return out;
    ++out.rows_done;   // row 16 float
    if (!float_var(objects.r17, 8, concmd)) return out;
    ++out.rows_done;   // row 17 float
    if (!float_var(objects.r18, 9, concmd)) return out;
    ++out.rows_done;   // row 18 float
    if (!float_var(objects.r19, 10, concmd)) return out;
    ++out.rows_done;   // row 19 float
    if (!bool_var(objects.r20, 9, concmd)) return out;
    ++out.rows_done;   // row 20 bool
    if (!float_var(objects.r21, 11, concmd)) return out;
    ++out.rows_done;   // row 21 float
    if (!float_var(objects.r22, 12, concmd)) return out;
    ++out.rows_done;   // row 22 float
    if (!float_var(objects.r23, 13, concmd)) return out;
    ++out.rows_done;   // row 23 float
    if (!float_var(objects.r24, 14, concmd)) return out;
    ++out.rows_done;   // row 24 float
    if (!float_var(objects.r25, 15, concmd)) return out;
    ++out.rows_done;   // row 25 float
    if (!bool_var(objects.r26, 10, concmd)) return out;
    ++out.rows_done;   // row 26 bool
    if (!float_var(objects.r27, 16, concmd)) return out;
    ++out.rows_done;   // row 27 float
    if (!float_var(objects.r28, 17, concmd)) return out;
    ++out.rows_done;   // row 28 float
    if (!float_var(objects.r29, 18, concmd)) return out;
    ++out.rows_done;   // row 29 float
    if (!float_var(objects.r30, 19, concmd)) return out;
    ++out.rows_done;   // row 30 float
    if (!float_var(objects.r31, 20, concmd)) return out;
    ++out.rows_done;   // row 31 float
    if (!float_var(objects.r32, 21, concmd)) return out;
    ++out.rows_done;   // row 32 float
    if (!float_var(objects.r33, 22, concmd)) return out;
    ++out.rows_done;   // row 33 float
    if (!slider_item(objects.r34, kSlot375MenuStrings[0], kSlot375MenuStringLengths[0], objects.r31, kSlot375SliderLoBits[0], kSlot375SliderHiBits[0], concmd, menu)) return out;
    ++out.rows_done;   // row 34 slider
    if (!slider_item(objects.r35, kSlot375MenuStrings[0], kSlot375MenuStringLengths[0], objects.r32, kSlot375SliderLoBits[1], kSlot375SliderHiBits[1], concmd, menu)) return out;
    ++out.rows_done;   // row 35 slider
    if (!slider_item(objects.r36, kSlot375MenuStrings[0], kSlot375MenuStringLengths[0], objects.r33, kSlot375SliderLoBits[2], kSlot375SliderHiBits[2], concmd, menu)) return out;
    ++out.rows_done;   // row 36 slider
    if (!family_named_item_26db0(objects.r37, kSlot375MenuStrings[0], kSlot375MenuStringLengths[0], objects.r09.base, target_of(objects.r09), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 37 tog
    if (!family_named_item_26db0(objects.r38, kSlot375MenuStrings[0], kSlot375MenuStringLengths[0], objects.r10.base, target_of(objects.r10), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 38 tog
    if (!family_group_item_26c10(objects.r39, kSlot375MenuStrings[0], kSlot375MenuStringLengths[0], kSlot375MenuStrings[1], kSlot375MenuStringLengths[1], kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 39 group
    if (!family_group_item_26c10(objects.r40, kSlot375MenuStrings[1], kSlot375MenuStringLengths[1], kSlot375MenuStrings[2], kSlot375MenuStringLengths[2], kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 40 group
    if (!family_toggle_item_26ea0(objects.r41, kSlot375MenuStrings[2], kSlot375MenuStringLengths[2], kSlot375MenuStrings[3], kSlot375MenuStringLengths[3], target_of(objects.r14), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 41 toge
    if (!family_toggle_item_26ea0(objects.r42, kSlot375MenuStrings[2], kSlot375MenuStringLengths[2], kSlot375MenuStrings[4], kSlot375MenuStringLengths[4], target_of(objects.r15), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 42 toge
    if (!family_toggle_item_26ea0(objects.r43, kSlot375MenuStrings[2], kSlot375MenuStringLengths[2], kSlot375MenuStrings[5], kSlot375MenuStringLengths[5], target_of(objects.r16), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 43 toge
    if (!family_toggle_item_26ea0(objects.r44, kSlot375MenuStrings[2], kSlot375MenuStringLengths[2], kSlot375MenuStrings[6], kSlot375MenuStringLengths[6], target_of(objects.r17), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 44 toge
    if (!family_toggle_item_26ea0(objects.r45, kSlot375MenuStrings[2], kSlot375MenuStringLengths[2], kSlot375MenuStrings[7], kSlot375MenuStringLengths[7], target_of(objects.r18), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 45 toge
    if (!family_toggle_item_26ea0(objects.r46, kSlot375MenuStrings[2], kSlot375MenuStringLengths[2], kSlot375MenuStrings[8], kSlot375MenuStringLengths[8], target_of(objects.r19), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 46 toge
    if (!family_toggle_item_26ea0(objects.r47, kSlot375MenuStrings[2], kSlot375MenuStringLengths[2], kSlot375MenuStrings[9], kSlot375MenuStringLengths[9], target_of(objects.r20), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 47 toge
    if (!family_toggle_item_26ea0(objects.r48, kSlot375MenuStrings[2], kSlot375MenuStringLengths[2], kSlot375MenuStrings[10], kSlot375MenuStringLengths[10], target_of(objects.r21), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 48 toge
    if (!family_toggle_item_26ea0(objects.r49, kSlot375MenuStrings[2], kSlot375MenuStringLengths[2], kSlot375MenuStrings[11], kSlot375MenuStringLengths[11], target_of(objects.r22), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 49 toge
    if (!family_toggle_item_26ea0(objects.r50, kSlot375MenuStrings[2], kSlot375MenuStringLengths[2], kSlot375MenuStrings[12], kSlot375MenuStringLengths[12], target_of(objects.r23), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 50 toge
    if (!family_toggle_item_26ea0(objects.r51, kSlot375MenuStrings[2], kSlot375MenuStringLengths[2], kSlot375MenuStrings[13], kSlot375MenuStringLengths[13], target_of(objects.r24), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 51 toge
    if (!family_group_item_26c10(objects.r52, kSlot375MenuStrings[1], kSlot375MenuStringLengths[1], kSlot375MenuStrings[14], kSlot375MenuStringLengths[14], kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 52 group
    if (!family_toggle_item_26ea0(objects.r53, kSlot375MenuStrings[14], kSlot375MenuStringLengths[14], kSlot375MenuStrings[15], kSlot375MenuStringLengths[15], target_of(objects.r26), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 53 toge
    if (!family_toggle_item_26ea0(objects.r54, kSlot375MenuStrings[14], kSlot375MenuStringLengths[14], kSlot375MenuStrings[16], kSlot375MenuStringLengths[16], target_of(objects.r27), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 54 toge
    if (!family_toggle_item_26ea0(objects.r55, kSlot375MenuStrings[14], kSlot375MenuStringLengths[14], kSlot375MenuStrings[17], kSlot375MenuStringLengths[17], target_of(objects.r28), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 55 toge
    if (!family_toggle_item_26ea0(objects.r56, kSlot375MenuStrings[14], kSlot375MenuStringLengths[14], kSlot375MenuStrings[18], kSlot375MenuStringLengths[18], target_of(objects.r29), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 56 toge
    if (!family_toggle_item_26ea0(objects.r57, kSlot375MenuStrings[14], kSlot375MenuStringLengths[14], kSlot375MenuStrings[19], kSlot375MenuStringLengths[19], target_of(objects.r30), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 57 toge
    if (!family_named_item_26db0(objects.r58, kSlot375MenuStrings[0], kSlot375MenuStringLengths[0], objects.r11.base, target_of(objects.r11), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 58 tog
    if (!family_named_item_26db0(objects.r59, kSlot375MenuStrings[0], kSlot375MenuStringLengths[0], objects.r12.base, target_of(objects.r12), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 59 tog
    if (!family_named_item_26db0(objects.r60, kSlot375MenuStrings[0], kSlot375MenuStringLengths[0], objects.r13.base, target_of(objects.r13), kSlot375ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 60 tog
    out.ok = true;
    return out;
}

}  // namespace sotc
