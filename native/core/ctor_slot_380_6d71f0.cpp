#include "ctor_slot_380_6d71f0.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot380ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}

[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot380FloatNames[n], "", true, kSlot380FloatLoBits[n], kSlot380FloatHiBits[n], kSlot380FloatValueBits[n], kSlot380FloatFnTag, kSlot380VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}


[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot380BoolNames[n], kSlot380BoolNameLengths[n], "", 0, kSlot380BoolFnTag, concmd, kSlot380BoolValues[n],
                                          kSlot380BoolValues[n]).ok;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot380Result init_slot380_6d71f0(Slot380Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot380Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!bool_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!family_named_item_26db0(objects.r02, kSlot380MenuStrings[0], kSlot380MenuStringLengths[0], objects.r01.base, target_of(objects.r01), kSlot380ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 02 tog
    if (!bool_var(objects.r03, 2, concmd)) return out;
    ++out.rows_done;   // row 03 bool
    if (!family_named_item_26db0(objects.r04, kSlot380MenuStrings[0], kSlot380MenuStringLengths[0], objects.r03.base, target_of(objects.r03), kSlot380ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 04 tog
    if (!bool_var(objects.r05, 3, concmd)) return out;
    ++out.rows_done;   // row 05 bool
    if (!family_named_item_26db0(objects.r06, kSlot380MenuStrings[0], kSlot380MenuStringLengths[0], objects.r05.base, target_of(objects.r05), kSlot380ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 06 tog
    if (!bool_var(objects.r07, 4, concmd)) return out;
    ++out.rows_done;   // row 07 bool
    if (!family_named_item_26db0(objects.r08, kSlot380MenuStrings[0], kSlot380MenuStringLengths[0], objects.r07.base, target_of(objects.r07), kSlot380ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 08 tog
    if (!bool_var(objects.r09, 5, concmd)) return out;
    ++out.rows_done;   // row 09 bool
    if (!family_named_item_26db0(objects.r10, kSlot380MenuStrings[1], kSlot380MenuStringLengths[1], objects.r09.base, target_of(objects.r09), kSlot380ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 10 tog
    if (!family_named_item_26db0(objects.r11, kSlot380MenuStrings[1], kSlot380MenuStringLengths[1], objects.r00.base, target_of(objects.r00), kSlot380ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 11 tog
    if (!float_var(objects.r12, 0, concmd)) return out;
    ++out.rows_done;   // row 12 float
    if (!slider_item(objects.r13, kSlot380MenuStrings[1], kSlot380MenuStringLengths[1], objects.r12, kSlot380SliderLoBits[0], kSlot380SliderHiBits[0], concmd, menu)) return out;
    ++out.rows_done;   // row 13 slider
    if (!bool_var(objects.r14, 6, concmd)) return out;
    ++out.rows_done;   // row 14 bool
    if (!family_named_item_26db0(objects.r15, kSlot380MenuStrings[1], kSlot380MenuStringLengths[1], objects.r14.base, target_of(objects.r14), kSlot380ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 15 tog
    if (!float_var(objects.r16, 1, concmd)) return out;
    ++out.rows_done;   // row 16 float
    if (!family_named_item_26db0(objects.r17, kSlot380MenuStrings[1], kSlot380MenuStringLengths[1], objects.r16.base, target_of(objects.r16), kSlot380ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 17 tog
    if (!bool_var(objects.r18, 7, concmd)) return out;
    ++out.rows_done;   // row 18 bool
    if (!family_named_item_26db0(objects.r19, kSlot380MenuStrings[1], kSlot380MenuStringLengths[1], objects.r18.base, target_of(objects.r18), kSlot380ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 19 tog
    if (!float_var(objects.r20, 2, concmd)) return out;
    ++out.rows_done;   // row 20 float
    if (!bool_var(objects.r21, 8, concmd)) return out;
    ++out.rows_done;   // row 21 bool
    if (!family_named_item_26db0(objects.r22, kSlot380MenuStrings[1], kSlot380MenuStringLengths[1], objects.r21.base, target_of(objects.r21), kSlot380ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 22 tog
    if (!bool_var(objects.r23, 9, concmd)) return out;
    ++out.rows_done;   // row 23 bool
    if (!family_named_item_26db0(objects.r24, kSlot380MenuStrings[1], kSlot380MenuStringLengths[1], objects.r23.base, target_of(objects.r23), kSlot380ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 24 tog
    if (!bool_var(objects.r25, 10, concmd)) return out;
    ++out.rows_done;   // row 25 bool
    if (!family_named_item_26db0(objects.r26, kSlot380MenuStrings[1], kSlot380MenuStringLengths[1], objects.r25.base, target_of(objects.r25), kSlot380ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 26 tog
    out.ok = true;
    return out;
}

}  // namespace sotc
