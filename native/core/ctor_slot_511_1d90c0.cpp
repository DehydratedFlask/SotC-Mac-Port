#include "ctor_slot_511_1d90c0.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot511ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}


[[maybe_unused]] bool uint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const HelperIntSiteSpec spec{kSlot511UIntNames[n], kSlot511UIntNameLengths[n], "", 0, 0, 0, kSlot511VarFlags, kSlot511UIntFnTag, kSlot511UIntValue[n], kSlot511UIntValue[n], kSlot511UIntEarly[n] != 0};
    return helper_init_unbounded_int_var_site(var, spec, concmd).ok;
}










[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot511Result init_slot511_1d90c0(Slot511Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot511Result out{};

    if (!uint_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 uint
    if (!uint_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 uint
    if (!uint_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 uint
    if (!uint_var(objects.r03, 3, concmd)) return out;
    ++out.rows_done;   // row 03 uint
    if (!family_named_item_26db0(objects.r04, kSlot511MenuStrings[0], kSlot511MenuStringLengths[0], objects.r00.base, unbounded_int_menu_target(objects.r00), kSlot511ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 04 tog
    if (!family_named_item_26db0(objects.r05, kSlot511MenuStrings[0], kSlot511MenuStringLengths[0], objects.r01.base, unbounded_int_menu_target(objects.r01), kSlot511ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 05 tog
    if (!family_named_item_26db0(objects.r06, kSlot511MenuStrings[0], kSlot511MenuStringLengths[0], objects.r02.base, unbounded_int_menu_target(objects.r02), kSlot511ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 06 tog
    if (!family_named_item_26db0(objects.r07, kSlot511MenuStrings[0], kSlot511MenuStringLengths[0], objects.r03.base, unbounded_int_menu_target(objects.r03), kSlot511ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 07 tog
    if (!build_empty_tree(objects.r08, kSlot511TreeNodeSize, objects.tree_alloc, kSlot511TreeDtorRva, concmd)) return out;
    ++out.rows_done;   // row 08 tree
    if (!build_empty_tree(objects.r09, kSlot511TreeNodeSize, objects.tree_alloc, kSlot511TreeDtorRva, concmd)) return out;
    ++out.rows_done;   // row 09 tree
    out.ok = true;
    return out;
}


}  // namespace sotc
