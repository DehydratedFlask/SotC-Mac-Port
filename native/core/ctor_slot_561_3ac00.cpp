#include "ctor_slot_561_3ac00.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot561ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}


[[maybe_unused]] bool uint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const HelperIntSiteSpec spec{kSlot561UIntNames[n], kSlot561UIntNameLengths[n], "", 0, 0, 0, kSlot561VarFlags, kSlot561UIntFnTag, kSlot561UIntValue[n], kSlot561UIntValue[n]};
    return helper_init_unbounded_int_var_site(var, spec, concmd).ok;
}




[[maybe_unused]] bool int_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const HelperIntSiteSpec spec{kSlot561IntNames[n], kSlot561IntNameLengths[n], "", 0, kSlot561IntMin[n], kSlot561IntMax[n], kSlot561VarFlags, kSlot561IntFnTag, kSlot561IntValue[n], kSlot561IntValue[n]};
    return helper_init_ranged_int_var_site(var, spec, concmd).ok;
}


[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot561BoolNames[n], kSlot561BoolNameLengths[n], "", 0, kSlot561BoolFnTag, concmd, kSlot561BoolValues[n],
                                          kSlot561BoolValues[n]).ok;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot561Result init_slot561_3ac00(Slot561Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot561Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!bool_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!int_var(objects.r02, 0, concmd)) return out;
    ++out.rows_done;   // row 02 int
    if (!uint_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 uint
    if (!family_group_item_26c10(objects.r04, kSlot561MenuStrings[0], kSlot561MenuStringLengths[0], kSlot561MenuStrings[1], kSlot561MenuStringLengths[1], kSlot561ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 04 group
    if (!family_toggle_item_26ea0(objects.r05, kSlot561MenuStrings[1], kSlot561MenuStringLengths[1], kSlot561MenuStrings[2], kSlot561MenuStringLengths[2], target_of(objects.r00), kSlot561ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 05 toge
    if (!family_toggle_item_26ea0(objects.r06, kSlot561MenuStrings[1], kSlot561MenuStringLengths[1], kSlot561MenuStrings[3], kSlot561MenuStringLengths[3], target_of(objects.r01), kSlot561ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 06 toge
    if (!family_toggle_item_26ea0(objects.r07, kSlot561MenuStrings[1], kSlot561MenuStringLengths[1], kSlot561MenuStrings[4], kSlot561MenuStringLengths[4], ranged_int_c580_menu_target(objects.r02), kSlot561ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 07 toge
    if (!family_toggle_item_26ea0(objects.r08, kSlot561MenuStrings[1], kSlot561MenuStringLengths[1], kSlot561MenuStrings[5], kSlot561MenuStringLengths[5], unbounded_int_menu_target(objects.r03), kSlot561ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 08 toge
    out.ok = true;
    return out;
}

}  // namespace sotc
