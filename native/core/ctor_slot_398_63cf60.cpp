#include "ctor_slot_398_63cf60.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot398ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}




[[maybe_unused]] bool lint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return helper_init_local_int_var_site(var, kSlot398LocalIntSites[n], empty_string_f730(), concmd).ok;
}

[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot398BoolNames[n], kSlot398BoolNameLengths[n], "", 0, kSlot398BoolFnTag, concmd, kSlot398BoolValues[n],
                                          kSlot398BoolValues[n]).ok;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot398Result init_slot398_63cf60(Slot398Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot398Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!family_named_item_26db0(objects.r01, kSlot398MenuStrings[0], kSlot398MenuStringLengths[0], objects.r00.base, target_of(objects.r00), kSlot398ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 01 tog
    if (!bool_var(objects.r02, 1, concmd)) return out;
    ++out.rows_done;   // row 02 bool
    if (!family_named_item_26db0(objects.r03, kSlot398MenuStrings[0], kSlot398MenuStringLengths[0], objects.r02.base, target_of(objects.r02), kSlot398ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 03 tog
    if (!lint_var(objects.r04, 0, concmd)) return out;
    ++out.rows_done;   // row 04 lint
    if (!lint_var(objects.r05, 1, concmd)) return out;
    ++out.rows_done;   // row 05 lint
    if (!family_toggle_item_26ea0(objects.r06, kSlot398MenuStrings[1], kSlot398MenuStringLengths[1], kSlot398MenuStrings[2], kSlot398MenuStringLengths[2], local_int_var_menu_target(objects.r04), kSlot398ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 06 toge
    if (!family_toggle_item_26ea0(objects.r07, kSlot398MenuStrings[1], kSlot398MenuStringLengths[1], kSlot398MenuStrings[3], kSlot398MenuStringLengths[3], local_int_var_menu_target(objects.r05), kSlot398ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 07 toge
    out.ok = true;
    return out;
}

}  // namespace sotc
