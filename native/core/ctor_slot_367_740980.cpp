#include "ctor_slot_367_740980.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot367ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}

[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot367FloatNames[n], "", true, kSlot367FloatLoBits[n], kSlot367FloatHiBits[n], kSlot367FloatValueBits[n], kSlot367FloatFnTag, kSlot367VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}

[[maybe_unused]] bool ufloat_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot367UFloatDesc[n];
    const char* const desc = d < 0 ? "" : kSlot367MenuStrings[d];
    const std::size_t desc_length = d < 0 ? 0 : kSlot367MenuStringLengths[d];
    const HelperFloatSiteSpec spec{kSlot367UFloatNames[n], kSlot367UFloatNameLengths[n], desc, desc_length, kSlot367VarFlags, kSlot367UFloatFnTag, kSlot367UFloatValueBits[n], kSlot367UFloatEarly[n] != 0};
    return helper_init_unbounded_float_var_site(var, spec, concmd).ok;
}



[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot367BoolNames[n], kSlot367BoolNameLengths[n], "", 0, kSlot367BoolFnTag, concmd, kSlot367BoolValues[n],
                                          kSlot367BoolValues[n]).ok;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot367Result init_slot367_740980(Slot367Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot367Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!bool_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!bool_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 bool
    if (!bool_var(objects.r03, 3, concmd)) return out;
    ++out.rows_done;   // row 03 bool
    if (!ufloat_var(objects.r04, 0, concmd)) return out;
    ++out.rows_done;   // row 04 ufloat
    if (!ufloat_var(objects.r05, 1, concmd)) return out;
    ++out.rows_done;   // row 05 ufloat
    if (!ufloat_var(objects.r06, 2, concmd)) return out;
    ++out.rows_done;   // row 06 ufloat
    if (!ufloat_var(objects.r07, 3, concmd)) return out;
    ++out.rows_done;   // row 07 ufloat
    if (!ufloat_var(objects.r08, 4, concmd)) return out;
    ++out.rows_done;   // row 08 ufloat
    if (!ufloat_var(objects.r09, 5, concmd)) return out;
    ++out.rows_done;   // row 09 ufloat
    if (!ufloat_var(objects.r10, 6, concmd)) return out;
    ++out.rows_done;   // row 10 ufloat
    if (!ufloat_var(objects.r11, 7, concmd)) return out;
    ++out.rows_done;   // row 11 ufloat
    if (!ufloat_var(objects.r12, 8, concmd)) return out;
    ++out.rows_done;   // row 12 ufloat
    if (!ufloat_var(objects.r13, 9, concmd)) return out;
    ++out.rows_done;   // row 13 ufloat
    if (!bool_var(objects.r14, 4, concmd)) return out;
    ++out.rows_done;   // row 14 bool
    if (!ufloat_var(objects.r15, 10, concmd)) return out;
    ++out.rows_done;   // row 15 ufloat
    if (!ufloat_var(objects.r16, 11, concmd)) return out;
    ++out.rows_done;   // row 16 ufloat
    if (!bool_var(objects.r17, 5, concmd)) return out;
    ++out.rows_done;   // row 17 bool
    if (!float_var(objects.r18, 0, concmd)) return out;
    ++out.rows_done;   // row 18 float
    out.ok = true;
    return out;
}

}  // namespace sotc
