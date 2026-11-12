#include "ctor_slot_445_4cfe80.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot445ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}











[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot445BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot445BoolNames[n], kSlot445BoolNameLengths[n], d < 0 ? "" : kSlot445MenuStrings[d], d < 0 ? 0 : kSlot445MenuStringLengths[d],
                                          kSlot445BoolFnTag, concmd, kSlot445BoolValues[n], kSlot445BoolValues[n], kSlot445BoolEarly[n] != 0).ok;
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

// Hash-to-dword store table (batch 20): every site hashes its rodata string natively (fnv_cstr_a2ddc0) and stores the dword; no atexit record exists for these sites.
bool hash_store_row(Slot445HashTable& t, [[maybe_unused]] ConCmdRegistry& concmd) noexcept {
    const HashStoreSpec spec{kSlot445HashStores, kSlot445HashStoreCount, kSlot445HashStringBlob, sizeof kSlot445HashStringBlob, kSlot445HashStringOffset, kSlot445HashStringCount};
    return replay_hash_stores(t.bytes, sizeof t.bytes, spec);
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot445Result init_slot445_4cfe80(Slot445Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot445Result out{};

    if (!name_holder(objects.r00, kSlot445MenuStrings[0], kSlot445MenuStringLengths[0], kSlot445HolderVptr, kSlot445HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 00 holder
    if (!name_holder(objects.r01, kSlot445MenuStrings[1], kSlot445MenuStringLengths[1], kSlot445HolderVptrB, kSlot445HolderDtorRvaB, concmd)) return out;
    ++out.rows_done;   // row 01 holder
    if (!name_holder(objects.r02, kSlot445MenuStrings[2], kSlot445MenuStringLengths[2], kSlot445HolderVptr, kSlot445HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 02 holder
    if (!name_holder(objects.r03, kSlot445MenuStrings[3], kSlot445MenuStringLengths[3], kSlot445HolderVptrB, kSlot445HolderDtorRvaB, concmd)) return out;
    ++out.rows_done;   // row 03 holder
    if (!bool_var(objects.r04, 0, concmd)) return out;
    ++out.rows_done;   // row 04 bool
    if (!hash_store_row(objects.r05, concmd)) return out;
    ++out.rows_done;   // row 05 hstab
    out.ok = true;
    return out;
}


}  // namespace sotc
