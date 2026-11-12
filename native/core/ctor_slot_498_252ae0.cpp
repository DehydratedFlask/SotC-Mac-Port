#include "ctor_slot_498_252ae0.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot498ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}


[[maybe_unused]] bool uint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const HelperIntSiteSpec spec{kSlot498UIntNames[n], kSlot498UIntNameLengths[n], "", 0, 0, 0, kSlot498VarFlags, kSlot498UIntFnTag, kSlot498UIntValue[n], kSlot498UIntValue[n], kSlot498UIntEarly[n] != 0};
    return helper_init_unbounded_int_var_site(var, spec, concmd).ok;
}







[[maybe_unused]] bool int_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const HelperIntSiteSpec spec{kSlot498IntNames[n], kSlot498IntNameLengths[n], "", 0, kSlot498IntMin[n], kSlot498IntMax[n], kSlot498VarFlags, kSlot498IntFnTag, kSlot498IntValue[n], kSlot498IntValue[n]};
    return helper_init_ranged_int_var_site(var, spec, concmd).ok;
}


[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot498BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot498BoolNames[n], kSlot498BoolNameLengths[n], d < 0 ? "" : kSlot498MenuStrings[d], d < 0 ? 0 : kSlot498MenuStringLengths[d],
                                          kSlot498BoolFnTag, concmd, kSlot498BoolValues[n], kSlot498BoolValues[n], kSlot498BoolEarly[n] != 0).ok;
}


// The 24 getter pairs and their {lo, hi, size} record stores (helper_498_member_hashes getters; no atexit record). Fails closed if a getter is not one of the pinned 48.
bool member_hash_block(Slot498MemberBlock& b) noexcept {
    for (std::size_t k = 0; k < kSlot498MemberCount; ++k) {
        std::uint32_t lo = 0;
        std::uint32_t hi = 0;
        if (!member_hash_getter_498(kSlot498MemberLoRva[k], lo) || !member_hash_getter_498(kSlot498MemberHiRva[k], hi)) return false;
        b.dw[3 * k] = lo;
        b.dw[3 * k + 1] = hi;
        b.dw[3 * k + 2] = kSlot498MemberSize[k];
    }
    return true;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot498Result init_slot498_252ae0(Slot498Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot498Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!bool_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!uint_var(objects.r02, 0, concmd)) return out;
    ++out.rows_done;   // row 02 uint
    if (!int_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 int
    if (!family_named_item_26db0(objects.r04, kSlot498MenuStrings[0], kSlot498MenuStringLengths[0], objects.r00.base, target_of(objects.r00), kSlot498ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 04 tog
    if (!family_named_item_26db0(objects.r05, kSlot498MenuStrings[0], kSlot498MenuStringLengths[0], objects.r01.base, target_of(objects.r01), kSlot498ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 05 tog
    if (!family_named_item_26db0(objects.r06, kSlot498MenuStrings[0], kSlot498MenuStringLengths[0], objects.r02.base, unbounded_int_menu_target(objects.r02), kSlot498ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 06 tog
    if (!family_named_item_26db0(objects.r07, kSlot498MenuStrings[0], kSlot498MenuStringLengths[0], objects.r03.base, ranged_int_c580_menu_target(objects.r03), kSlot498ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 07 tog
    if (!family_separator_item_26b60(objects.r08, kSlot498MenuStrings[0], kSlot498MenuStringLengths[0], kSlot498ItemFlags, objects.separator_alloc, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 08 sep
    if (!member_hash_block(objects.r09)) return out;
    ++out.rows_done;   // row 09 gettable
    out.ok = true;
    return out;
}


}  // namespace sotc
