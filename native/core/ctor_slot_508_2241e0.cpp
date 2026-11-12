#include "ctor_slot_508_2241e0.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot508ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}


[[maybe_unused]] bool uint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const HelperIntSiteSpec spec{kSlot508UIntNames[n], kSlot508UIntNameLengths[n], "", 0, 0, 0, kSlot508VarFlags, kSlot508UIntFnTag, kSlot508UIntValue[n], kSlot508UIntValue[n]};
    return helper_init_unbounded_int_var_site(var, spec, concmd).ok;
}




[[maybe_unused]] bool ufloat_var(ConCmdRangedVar& var, std::size_t n, const std::uint32_t* source_bits, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot508UFloatDesc[n];
    const char* const desc = d < 0 ? "" : kSlot508MenuStrings[d];
    const std::size_t desc_length = d < 0 ? 0 : kSlot508MenuStringLengths[d];
    const std::int32_t src = kSlot508UFloatSource[n];   // >= 0: the initial value is a run-time load of a global (an input), else a constant of the body
    const std::uint32_t value_bits = src < 0 ? kSlot508UFloatValueBits[n] : source_bits[src];
    const HelperFloatSiteSpec spec{kSlot508UFloatNames[n], kSlot508UFloatNameLengths[n], desc, desc_length, kSlot508VarFlags, kSlot508UFloatFnTag, value_bits, kSlot508UFloatEarly[n] != 0};
    return helper_init_unbounded_float_var_site(var, spec, concmd).ok;
}


[[maybe_unused]] bool int_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const HelperIntSiteSpec spec{kSlot508IntNames[n], kSlot508IntNameLengths[n], "", 0, kSlot508IntMin[n], kSlot508IntMax[n], kSlot508VarFlags, kSlot508IntFnTag, kSlot508IntValue[n], kSlot508IntValue[n]};
    return helper_init_ranged_int_var_site(var, spec, concmd).ok;
}


[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot508BoolNames[n], kSlot508BoolNameLengths[n], "", 0, kSlot508BoolFnTag, concmd, kSlot508BoolValues[n],
                                          kSlot508BoolValues[n]).ok;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot508Result init_slot508_2241e0(Slot508Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot508Result out{};

    if (!int_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 int
    if (!int_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 int
    if (!uint_var(objects.r02, 0, concmd)) return out;
    ++out.rows_done;   // row 02 uint
    if (!uint_var(objects.r03, 1, concmd)) return out;
    ++out.rows_done;   // row 03 uint
    if (!int_var(objects.r04, 2, concmd)) return out;
    ++out.rows_done;   // row 04 int
    if (!bool_var(objects.r05, 0, concmd)) return out;
    ++out.rows_done;   // row 05 bool
    if (!bool_var(objects.r06, 1, concmd)) return out;
    ++out.rows_done;   // row 06 bool
    if (!ufloat_var(objects.r07, 0, objects.source_bits, concmd)) return out;
    ++out.rows_done;   // row 07 ufloat
    if (!ufloat_var(objects.r08, 1, objects.source_bits, concmd)) return out;
    ++out.rows_done;   // row 08 ufloat
    if (!ufloat_var(objects.r09, 2, objects.source_bits, concmd)) return out;
    ++out.rows_done;   // row 09 ufloat
    if (!ufloat_var(objects.r10, 3, objects.source_bits, concmd)) return out;
    ++out.rows_done;   // row 10 ufloat
    if (!ufloat_var(objects.r11, 4, objects.source_bits, concmd)) return out;
    ++out.rows_done;   // row 11 ufloat
    out.ok = true;
    return out;
}


}  // namespace sotc
