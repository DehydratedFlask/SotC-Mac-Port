#include "ctor_slot_354_79e180.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot354ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}

[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot354FloatNames[n], "", true, kSlot354FloatLoBits[n], kSlot354FloatHiBits[n], kSlot354FloatValueBits[n], kSlot354FloatFnTag, kSlot354VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}

[[maybe_unused]] bool ufloat_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot354UFloatDesc[n];
    const char* const desc = d < 0 ? "" : kSlot354MenuStrings[d];
    const std::size_t desc_length = d < 0 ? 0 : kSlot354MenuStringLengths[d];
    const HelperFloatSiteSpec spec{kSlot354UFloatNames[n], kSlot354UFloatNameLengths[n], desc, desc_length, kSlot354VarFlags, kSlot354UFloatFnTag, kSlot354UFloatValueBits[n], kSlot354UFloatEarly[n] != 0};
    return helper_init_unbounded_float_var_site(var, spec, concmd).ok;
}

[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot354BoolNames[n], kSlot354BoolNameLengths[n], "", 0, kSlot354BoolFnTag, concmd, kSlot354BoolValues[n],
                                          kSlot354BoolValues[n]).ok;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot354Result init_slot354_79e180(Slot354Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot354Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!bool_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!float_var(objects.r02, 0, concmd)) return out;
    ++out.rows_done;   // row 02 float
    if (!bool_var(objects.r03, 2, concmd)) return out;
    ++out.rows_done;   // row 03 bool
    if (!family_toggle_item_26ea0(objects.r04, kSlot354MenuStrings[0], kSlot354MenuStringLengths[0], kSlot354MenuStrings[1], kSlot354MenuStringLengths[1], target_of(objects.r03), kSlot354ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 04 toge
    if (!ufloat_var(objects.r05, 0, concmd)) return out;
    ++out.rows_done;   // row 05 ufloat
    if (!ufloat_var(objects.r06, 1, concmd)) return out;
    ++out.rows_done;   // row 06 ufloat
    if (!ufloat_var(objects.r07, 2, concmd)) return out;
    ++out.rows_done;   // row 07 ufloat
    if (!ufloat_var(objects.r08, 3, concmd)) return out;
    ++out.rows_done;   // row 08 ufloat
    out.ok = true;
    return out;
}

}  // namespace sotc
