#include "ctor_slot_363_751c70.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot363ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}




[[maybe_unused]] bool lint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return helper_init_local_int_var_site(var, kSlot363LocalIntSites[n], empty_string_f730(), concmd).ok;
}


[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot363Result init_slot363_751c70(Slot363Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot363Result out{};

    if (!lint_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 lint
    if (!lint_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 lint
    if (!lint_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 lint
    out.ok = true;
    return out;
}

}  // namespace sotc
