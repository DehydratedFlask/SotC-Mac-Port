#include "ctor_slot_382_6c8600.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot382ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}

[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot382FloatNames[n], "", true, kSlot382FloatLoBits[n], kSlot382FloatHiBits[n], kSlot382FloatValueBits[n], kSlot382FloatFnTag, kSlot382VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}


[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot382BoolNames[n], kSlot382BoolNameLengths[n], "", 0, kSlot382BoolFnTag, concmd, kSlot382BoolValues[n],
                                          kSlot382BoolValues[n]).ok;
}

// `[cell] = name; cell->hash = 0x52a990(name)`; the four bytes at +0x0c are never written.
bool name_cell(NameHashCell& cell, const char* name) noexcept {
    cell.name = name;
    cell.hash = fnv_cstr_52a990(name);
    return true;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot382Result init_slot382_6c8600(Slot382Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot382Result out{};

    if (!name_cell(objects.r00, kSlot382MenuStrings[0])) return out;
    ++out.rows_done;   // row 00 cell
    if (!float_var(objects.r01, 0, concmd)) return out;
    ++out.rows_done;   // row 01 float
    if (!float_var(objects.r02, 1, concmd)) return out;
    ++out.rows_done;   // row 02 float
    if (!bool_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 bool
    if (!float_var(objects.r04, 2, concmd)) return out;
    ++out.rows_done;   // row 04 float
    if (!float_var(objects.r05, 3, concmd)) return out;
    ++out.rows_done;   // row 05 float
    if (!family_named_item_26db0(objects.r06, kSlot382MenuStrings[1], kSlot382MenuStringLengths[1], objects.r02.base, target_of(objects.r02), kSlot382ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 06 tog
    if (!family_named_item_26db0(objects.r07, kSlot382MenuStrings[2], kSlot382MenuStringLengths[2], objects.r03.base, target_of(objects.r03), kSlot382ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 07 tog
    if (!family_named_item_26db0(objects.r08, kSlot382MenuStrings[2], kSlot382MenuStringLengths[2], objects.r04.base, target_of(objects.r04), kSlot382ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 08 tog
    if (!family_named_item_26db0(objects.r09, kSlot382MenuStrings[2], kSlot382MenuStringLengths[2], objects.r05.base, target_of(objects.r05), kSlot382ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 09 tog
    out.ok = true;
    return out;
}

}  // namespace sotc
