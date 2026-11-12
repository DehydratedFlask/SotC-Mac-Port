#include "ctor_slot_401_621530.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot401ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}




[[maybe_unused]] bool lint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return helper_init_local_int_var_site(var, kSlot401LocalIntSites[n], empty_string_f730(), concmd).ok;
}

[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot401BoolNames[n], kSlot401BoolNameLengths[n], "", 0, kSlot401BoolFnTag, concmd, kSlot401BoolValues[n],
                                          kSlot401BoolValues[n]).ok;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot401Result init_slot401_621530(Slot401Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot401Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!bool_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!bool_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 bool
    if (!lint_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 lint
    if (!lint_var(objects.r04, 1, concmd)) return out;
    ++out.rows_done;   // row 04 lint
    if (!bool_var(objects.r05, 3, concmd)) return out;
    ++out.rows_done;   // row 05 bool
    if (!bool_var(objects.r06, 4, concmd)) return out;
    ++out.rows_done;   // row 06 bool
    if (!family_named_item_26db0(objects.r07, kSlot401MenuStrings[0], kSlot401MenuStringLengths[0], objects.r02.base, target_of(objects.r02), kSlot401ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 07 tog
    if (!family_named_item_26db0(objects.r08, kSlot401MenuStrings[0], kSlot401MenuStringLengths[0], objects.r03.base, local_int_var_menu_target(objects.r03), kSlot401ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 08 tog
    if (!family_named_item_26db0(objects.r09, kSlot401MenuStrings[0], kSlot401MenuStringLengths[0], objects.r04.base, local_int_var_menu_target(objects.r04), kSlot401ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 09 tog
    if (!family_named_item_26db0(objects.r10, kSlot401MenuStrings[0], kSlot401MenuStringLengths[0], objects.r00.base, target_of(objects.r00), kSlot401ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 10 tog
    if (!family_named_item_26db0(objects.r11, kSlot401MenuStrings[0], kSlot401MenuStringLengths[0], objects.r01.base, target_of(objects.r01), kSlot401ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 11 tog
    if (!family_named_item_26db0(objects.r12, kSlot401MenuStrings[1], kSlot401MenuStringLengths[1], objects.r06.base, target_of(objects.r06), kSlot401ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 12 tog
    if (!bool_var(objects.r13, 5, concmd)) return out;
    ++out.rows_done;   // row 13 bool
    if (!family_named_item_26db0(objects.r14, kSlot401MenuStrings[0], kSlot401MenuStringLengths[0], objects.r13.base, target_of(objects.r13), kSlot401ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 14 tog
    if (!bool_var(objects.r15, 6, concmd)) return out;
    ++out.rows_done;   // row 15 bool
    if (!family_named_item_26db0(objects.r16, kSlot401MenuStrings[0], kSlot401MenuStringLengths[0], objects.r15.base, target_of(objects.r15), kSlot401ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 16 tog
    if (!bool_var(objects.r17, 7, concmd)) return out;
    ++out.rows_done;   // row 17 bool
    if (!family_named_item_26db0(objects.r18, kSlot401MenuStrings[0], kSlot401MenuStringLengths[0], objects.r17.base, target_of(objects.r17), kSlot401ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 18 tog
    if (!bool_var(objects.r19, 8, concmd)) return out;
    ++out.rows_done;   // row 19 bool
    if (!family_named_item_26db0(objects.r20, kSlot401MenuStrings[0], kSlot401MenuStringLengths[0], objects.r19.base, target_of(objects.r19), kSlot401ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 20 tog
    if (!bool_var(objects.r21, 9, concmd)) return out;
    ++out.rows_done;   // row 21 bool
    if (!family_named_item_26db0(objects.r22, kSlot401MenuStrings[0], kSlot401MenuStringLengths[0], objects.r21.base, target_of(objects.r21), kSlot401ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 22 tog
    out.ok = true;
    return out;
}

}  // namespace sotc
