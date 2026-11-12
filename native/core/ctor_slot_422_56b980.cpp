#include "ctor_slot_422_56b980.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot422ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}









[[maybe_unused]] bool lint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const SharedStr view = kSlot422LintDesc[n] == nullptr ? empty_string_f730() : family_view(kSlot422LintDesc[n], kSlot422LintDescLength[n]);
    return helper_init_local_int_var_site(var, kSlot422LocalIntSites[n], view, concmd).ok;
}

[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot422BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot422BoolNames[n], kSlot422BoolNameLengths[n], d < 0 ? "" : kSlot422MenuStrings[d], d < 0 ? 0 : kSlot422MenuStringLengths[d],
                                          kSlot422BoolFnTag, concmd, kSlot422BoolValues[n], kSlot422BoolValues[n], kSlot422BoolEarly[n] != 0).ok;
}

[[maybe_unused]] bool boolx_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const BoolVar274730SiteResult r = helper_init_bool_var_274730_site(var, kSlot422BoolXSites[n], concmd);   // 0x274730 ; call [argfn + 0x18] ; f590(name) ; __cxa_atexit(0x20b0)
    return r.ok && r.refusal == HelperRefusal::none;
}

// Zero pointer slot: the original stores qword 0 and records __cxa_atexit(0x565e80, &slot) (identity tag; the destructor frees the pointee, never run).
bool zero_pointer(std::uint64_t& slot, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {
    slot = 0;
    return concmd.atexit_log.record(dtor, &slot);
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot422Result init_slot422_56b980(Slot422Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot422Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!bool_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!bool_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 bool
    if (!bool_var(objects.r03, 3, concmd)) return out;
    ++out.rows_done;   // row 03 bool
    if (!bool_var(objects.r04, 4, concmd)) return out;
    ++out.rows_done;   // row 04 bool
    if (!bool_var(objects.r05, 5, concmd)) return out;
    ++out.rows_done;   // row 05 bool
    if (!bool_var(objects.r06, 6, concmd)) return out;
    ++out.rows_done;   // row 06 bool
    if (!lint_var(objects.r07, 0, concmd)) return out;
    ++out.rows_done;   // row 07 lint
    if (!lint_var(objects.r08, 1, concmd)) return out;
    ++out.rows_done;   // row 08 lint
    if (!boolx_var(objects.r09, 0, concmd)) return out;
    ++out.rows_done;   // row 09 boolx
    if (!lint_var(objects.r10, 2, concmd)) return out;
    ++out.rows_done;   // row 10 lint
    if (!build_node_ref_row(objects.r11, kSlot422NodeRefDtorRva, concmd)) return out;
    ++out.rows_done;   // row 11 nref
    if (!zero_pointer(objects.r12, kSlot422ZeroPtrDtorRva, concmd)) return out;
    ++out.rows_done;   // row 12 zq
    out.ok = true;
    return out;
}


}  // namespace sotc
