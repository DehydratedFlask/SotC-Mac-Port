#include "ctor_slot_390_691c50.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot390ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}










[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot390BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot390BoolNames[n], kSlot390BoolNameLengths[n], d < 0 ? "" : kSlot390MenuStrings[d], d < 0 ? 0 : kSlot390MenuStringLengths[d],
                                          kSlot390BoolFnTag, concmd, kSlot390BoolValues[n], kSlot390BoolValues[n]).ok;
}

// The 0x30 byte holder: view copy at +8, zero tail at +0x28, vptr identity tag; the stack view is released by an exact no-op f590 and
// __cxa_atexit(dtor, holder) is recorded (dtor 0x8ef00 or 0x2fced0 per holder class).
bool name_holder(NameHolder30& holder, const char* name, std::size_t length, std::uint64_t vptr, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {
    const SharedStr view = family_view(name, length);
    holder.name = view;
    holder.tail = 0;
    holder.vptr = vptr;
    const bool inert = release_inert_view_f590(view);
    const bool recorded = concmd.atexit_log.record(dtor, &holder);
    return inert && recorded;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot390Result init_slot390_691c50(Slot390Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot390Result out{};

    if (!build_empty_tree(objects.r00, kSlot390TreeNodeSize, objects.tree_alloc, kSlot390TreeDtorRva, concmd)) return out;
    ++out.rows_done;   // row 00 tree
    if (!build_node_ref_row(objects.r01, kSlot390NodeRefDtorRva, concmd)) return out;
    ++out.rows_done;   // row 01 nref
    if (!name_holder(objects.r02, kSlot390MenuStrings[0], kSlot390MenuStringLengths[0], kSlot390HolderVptr, kSlot390HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 02 holder
    if (!name_holder(objects.r03, kSlot390MenuStrings[1], kSlot390MenuStringLengths[1], kSlot390HolderVptr, kSlot390HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 03 holder
    if (!name_holder(objects.r04, kSlot390MenuStrings[2], kSlot390MenuStringLengths[2], kSlot390HolderVptr, kSlot390HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 04 holder
    if (!bool_var(objects.r05, 0, concmd)) return out;
    ++out.rows_done;   // row 05 bool
    if (!bool_var(objects.r06, 1, concmd)) return out;
    ++out.rows_done;   // row 06 bool
    out.ok = true;
    return out;
}


}  // namespace sotc
