#include "ctor_slot_493_277560.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot493ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}










[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot493BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot493BoolNames[n], kSlot493BoolNameLengths[n], d < 0 ? "" : kSlot493MenuStrings[d], d < 0 ? 0 : kSlot493MenuStringLengths[d],
                                          kSlot493BoolFnTag, concmd, kSlot493BoolValues[n], kSlot493BoolValues[n], kSlot493BoolEarly[n] != 0).ok;
}

[[maybe_unused]] bool boolx_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const BoolVar274730SiteResult r = helper_init_bool_var_274730_site(var, kSlot493BoolXSites[n], concmd);   // 0x274730 ; call [argfn + 0x18] ; f590(name) ; __cxa_atexit(0x20b0)
    return r.ok && r.refusal == HelperRefusal::none;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot493Result init_slot493_277560(Slot493Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot493Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!family_toggle_item_26ea0(objects.r01, kSlot493MenuStrings[0], kSlot493MenuStringLengths[0], kSlot493MenuStrings[1], kSlot493MenuStringLengths[1], target_of(objects.r00), kSlot493ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 01 toge
    if (!boolx_var(objects.r02, 0, concmd)) return out;
    ++out.rows_done;   // row 02 boolx
    if (!bool_var(objects.r03, 1, concmd)) return out;
    ++out.rows_done;   // row 03 bool
    if (!family_named_item_26db0(objects.r04, kSlot493MenuStrings[2], kSlot493MenuStringLengths[2], objects.r02.base, target_of(objects.r02), kSlot493ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 04 tog
    if (!family_named_item_26db0(objects.r05, kSlot493MenuStrings[2], kSlot493MenuStringLengths[2], objects.r03.base, target_of(objects.r03), kSlot493ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 05 tog
    out.ok = true;
    return out;
}


}  // namespace sotc
