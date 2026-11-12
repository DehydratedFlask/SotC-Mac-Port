#include "ctor_slot_352_7a2390.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot352ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}

[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot352FloatNames[n], "", true, kSlot352FloatLoBits[n], kSlot352FloatHiBits[n], kSlot352FloatValueBits[n], kSlot352FloatFnTag, kSlot352VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}

[[maybe_unused]] bool ufloat_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot352UFloatDesc[n];
    const char* const desc = d < 0 ? "" : kSlot352MenuStrings[d];
    const std::size_t desc_length = d < 0 ? 0 : kSlot352MenuStringLengths[d];
    const HelperFloatSiteSpec spec{kSlot352UFloatNames[n], kSlot352UFloatNameLengths[n], desc, desc_length, kSlot352VarFlags, kSlot352UFloatFnTag, kSlot352UFloatValueBits[n], kSlot352UFloatEarly[n] != 0};
    return helper_init_unbounded_float_var_site(var, spec, concmd).ok;
}

[[maybe_unused]] bool int_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const HelperIntSiteSpec spec{kSlot352IntNames[n], kSlot352IntNameLengths[n], "", 0, kSlot352IntMin[n], kSlot352IntMax[n], kSlot352VarFlags, kSlot352IntFnTag, kSlot352IntValue[n], kSlot352IntValue[n]};
    return helper_init_ranged_int_var_site(var, spec, concmd).ok;
}


[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot352Result init_slot352_7a2390(Slot352Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot352Result out{};

    if (!float_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 float
    if (!float_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 float
    if (!float_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 float
    if (!int_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 int
    if (!float_var(objects.r04, 3, concmd)) return out;
    ++out.rows_done;   // row 04 float
    if (!float_var(objects.r05, 4, concmd)) return out;
    ++out.rows_done;   // row 05 float
    if (!float_var(objects.r06, 5, concmd)) return out;
    ++out.rows_done;   // row 06 float
    if (!ufloat_var(objects.r07, 0, concmd)) return out;
    ++out.rows_done;   // row 07 ufloat
    out.ok = true;
    return out;
}

}  // namespace sotc
