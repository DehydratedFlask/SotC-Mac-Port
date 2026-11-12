#include "ctor_slot_373_7081e0.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot373ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}


[[maybe_unused]] bool ufloat_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot373UFloatDesc[n];
    const char* const desc = d < 0 ? "" : kSlot373MenuStrings[d];
    const std::size_t desc_length = d < 0 ? 0 : kSlot373MenuStringLengths[d];
    const HelperFloatSiteSpec spec{kSlot373UFloatNames[n], kSlot373UFloatNameLengths[n], desc, desc_length, kSlot373VarFlags, kSlot373UFloatFnTag, kSlot373UFloatValueBits[n], kSlot373UFloatEarly[n] != 0};
    return helper_init_unbounded_float_var_site(var, spec, concmd).ok;
}

[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot373BoolNames[n], kSlot373BoolNameLengths[n], "", 0, kSlot373BoolFnTag, concmd, kSlot373BoolValues[n],
                                          kSlot373BoolValues[n]).ok;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot373Result init_slot373_7081e0(Slot373Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot373Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!bool_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!bool_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 bool
    if (!ufloat_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 ufloat
    if (!ufloat_var(objects.r04, 1, concmd)) return out;
    ++out.rows_done;   // row 04 ufloat
    if (!ufloat_var(objects.r05, 2, concmd)) return out;
    ++out.rows_done;   // row 05 ufloat
    if (!ufloat_var(objects.r06, 3, concmd)) return out;
    ++out.rows_done;   // row 06 ufloat
    if (!family_named_item_26db0(objects.r07, kSlot373MenuStrings[0], kSlot373MenuStringLengths[0], objects.r02.base, target_of(objects.r02), kSlot373ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 07 tog
    if (!family_named_item_26db0(objects.r08, kSlot373MenuStrings[0], kSlot373MenuStringLengths[0], objects.r03.base, target_of(objects.r03), kSlot373ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 08 tog
    if (!family_named_item_26db0(objects.r09, kSlot373MenuStrings[0], kSlot373MenuStringLengths[0], objects.r04.base, target_of(objects.r04), kSlot373ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 09 tog
    if (!family_named_item_26db0(objects.r10, kSlot373MenuStrings[0], kSlot373MenuStringLengths[0], objects.r05.base, target_of(objects.r05), kSlot373ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 10 tog
    if (!family_named_item_26db0(objects.r11, kSlot373MenuStrings[0], kSlot373MenuStringLengths[0], objects.r06.base, target_of(objects.r06), kSlot373ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 11 tog
    out.ok = true;
    return out;
}

}  // namespace sotc
