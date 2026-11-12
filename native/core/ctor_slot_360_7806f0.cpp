#include "ctor_slot_360_7806f0.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot360ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}

[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot360FloatNames[n], "", true, kSlot360FloatLoBits[n], kSlot360FloatHiBits[n], kSlot360FloatValueBits[n], kSlot360FloatFnTag, kSlot360VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}



// `[cell] = name; cell->hash = 0x52a990(name)`; the four bytes at +0x0c are never written.
bool name_cell(NameHashCell& cell, const char* name) noexcept {
    cell.name = name;
    cell.hash = fnv_cstr_52a990(name);
    return true;
}

// The 0x30 byte holder: view copy at +8, zero tail at +0x28, vptr identity tag; the stack view is released by an exact no-op f590 and
// __cxa_atexit(0x8ef00, holder) is recorded.
bool name_holder(NameHolder30& holder, const char* name, std::size_t length, ConCmdRegistry& concmd) noexcept {
    const SharedStr view = family_view(name, length);
    holder.name = view;
    holder.tail = 0;
    holder.vptr = kSlot360HolderVptr;
    const bool inert = release_inert_view_f590(view);
    const bool recorded = concmd.atexit_log.record(kSlot360HolderDtorRva, &holder);
    return inert && recorded;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot360Result init_slot360_7806f0(Slot360Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot360Result out{};

    if (!float_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 float
    if (!float_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 float
    if (!float_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 float
    if (!float_var(objects.r03, 3, concmd)) return out;
    ++out.rows_done;   // row 03 float
    if (!family_named_item_26db0(objects.r04, kSlot360MenuStrings[0], kSlot360MenuStringLengths[0], objects.r00.base, target_of(objects.r00), kSlot360ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 04 tog
    if (!family_named_item_26db0(objects.r05, kSlot360MenuStrings[0], kSlot360MenuStringLengths[0], objects.r01.base, target_of(objects.r01), kSlot360ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 05 tog
    if (!family_named_item_26db0(objects.r06, kSlot360MenuStrings[0], kSlot360MenuStringLengths[0], objects.r02.base, target_of(objects.r02), kSlot360ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 06 tog
    if (!family_named_item_26db0(objects.r07, kSlot360MenuStrings[0], kSlot360MenuStringLengths[0], objects.r03.base, target_of(objects.r03), kSlot360ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 07 tog
    if (!name_holder(objects.r08, kSlot360MenuStrings[1], kSlot360MenuStringLengths[1], concmd)) return out;
    ++out.rows_done;   // row 08 holder
    if (!name_cell(objects.r09, kSlot360MenuStrings[2])) return out;
    ++out.rows_done;   // row 09 cell
    out.ok = true;
    return out;
}

}  // namespace sotc
