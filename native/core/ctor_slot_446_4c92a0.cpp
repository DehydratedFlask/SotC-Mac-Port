#include "ctor_slot_446_4c92a0.hpp"

#include <atomic>
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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot446ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}










[[maybe_unused]] bool lint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const SharedStr view = kSlot446LintDesc[n] == nullptr ? empty_string_f730() : family_view(kSlot446LintDesc[n], kSlot446LintDescLength[n]);
    return helper_init_local_int_var_site(var, kSlot446LocalIntSites[n], view, concmd).ok;
}

[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot446BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot446BoolNames[n], kSlot446BoolNameLengths[n], d < 0 ? "" : kSlot446MenuStrings[d], d < 0 ? 0 : kSlot446MenuStringLengths[d],
                                          kSlot446BoolFnTag, concmd, kSlot446BoolValues[n], kSlot446BoolValues[n], kSlot446BoolEarly[n] != 0).ok;
}


// `[cell] = name; cell->hash = 0x52a990(name)`; the four bytes at +0x0c are never written.
bool name_cell(NameHashCell& cell, const char* name) noexcept {
    cell.name = name;
    cell.hash = fnv_cstr_52a990(name);
    return true;
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

// Scripted-command registry entry: two borrowed name views (0xf2f0), the 0x88 B object image (three {obj+0x88, functor tag} pairs, zeros, the name SharedStr at +0x60 with an atomic control
// increment when a control block exists, the previous list head at +0x80), the push on the global list (cell 0x2ef6400), the registration 0x3be2d0(*0x2ef63f8, obj) when the registry
// object exists, the two view releases (description first) and __cxa_atexit(0x3c15e0, obj) (identity tag, never run). +0x88 is never written.
bool entry_row(Slot446Entry& e, std::size_t n, const RbHooks& hooks, ConCmdRegistry& concmd) noexcept {
    const SharedStr name = family_view(kSlot446EntryNames[n], kSlot446EntryNameLengths[n]);
    const SharedStr desc = family_view(kSlot446EntryDescs[n], kSlot446EntryDescLengths[n]);
    const std::uint64_t self88 = static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(e.b)) + 0x88;
    auto put = [&](std::size_t off, std::uint64_t v) { std::memcpy(e.b + off, &v, sizeof v); };
    put(0x00, self88);
    put(0x08, kSlot446EntryFns[3 * n + 0]);
    put(0x10, 0);
    put(0x18, 0);
    put(0x20, self88);
    put(0x28, kSlot446EntryFns[3 * n + 1]);
    put(0x30, 0);
    put(0x38, 0);
    put(0x40, self88);
    put(0x48, kSlot446EntryFns[3 * n + 2]);
    put(0x50, 0);
    put(0x58, 0);
    std::memcpy(e.b + 0x60, &name, sizeof name);
    if (name.ctl != nullptr) reinterpret_cast<std::atomic<std::int64_t>*>(name.ctl)->fetch_add(1, std::memory_order_seq_cst);   // lock incq
    void*& head_cell = ctor_98a90_list_head_cell_2ef6400();
    void* const registry = ctor_98a90_self_cell_2ef63f8();
    void* const previous_head = head_cell;
    head_cell = e.b;
    std::memcpy(e.b + 0x80, &previous_head, sizeof previous_head);
    if (registry != nullptr && !rb_register_entry_3be2d0(*static_cast<RbMap*>(registry), e.b, hooks).ok) return false;
    if (!release_inert_view_f590(desc)) return false;
    if (!release_inert_view_f590(name)) return false;
    return concmd.atexit_log.record(kSlot446EntryDtorRva, &e);
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot446Result init_slot446_4c92a0(Slot446Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot446Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!bool_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!name_holder(objects.r02, kSlot446MenuStrings[0], kSlot446MenuStringLengths[0], kSlot446HolderVptr, kSlot446HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 02 holder
    if (!bool_var(objects.r03, 2, concmd)) return out;
    ++out.rows_done;   // row 03 bool
    if (!bool_var(objects.r04, 3, concmd)) return out;
    ++out.rows_done;   // row 04 bool
    if (!name_cell(objects.r05, kSlot446MenuStrings[3])) return out;
    ++out.rows_done;   // row 05 cell
    if (!lint_var(objects.r06, 0, concmd)) return out;
    ++out.rows_done;   // row 06 lint
    if (!family_group_item_26c10(objects.r07, kSlot446MenuStrings[4], kSlot446MenuStringLengths[4], kSlot446MenuStrings[5], kSlot446MenuStringLengths[5], kSlot446ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 07 group
    if (!family_toggle_item_26ea0(objects.r08, kSlot446MenuStrings[5], kSlot446MenuStringLengths[5], kSlot446MenuStrings[6], kSlot446MenuStringLengths[6], local_int_var_menu_target(objects.r06), kSlot446ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 08 toge
    if (!entry_row(objects.r09, 0, objects.entry_hooks, concmd)) return out;
    ++out.rows_done;   // row 09 entry
    if (!entry_row(objects.r10, 1, objects.entry_hooks, concmd)) return out;
    ++out.rows_done;   // row 10 entry
    out.ok = true;
    return out;
}


}  // namespace sotc
