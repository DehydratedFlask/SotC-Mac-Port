#include "ctor_slot_358_786f50.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot358ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}


[[maybe_unused]] bool uint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const HelperIntSiteSpec spec{kSlot358UIntNames[n], kSlot358UIntNameLengths[n], "", 0, 0, 0, kSlot358VarFlags, kSlot358UIntFnTag, kSlot358UIntValue[n], kSlot358UIntValue[n]};
    return helper_init_unbounded_int_var_site(var, spec, concmd).ok;
}

// Console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], two f590, __cxa_atexit(0x4b0)); the handler is an identity tag, never called.
[[maybe_unused]] bool cmd_var(ConCmdCommand& command, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot358CmdDesc[n];
    const FConCmdCommandSpec spec{kSlot358CmdNames[n], kSlot358MenuStrings[d], kSlot358MenuStringLengths[d], kSlot358CmdHandler[n]};
    const FConCmdBuildResult built = init_fconcmd_object(command, spec, concmd);
    return built.ok && built.vtable_register_slot_ok && built.views_inert && built.atexit_recorded && !built.handler_invoked;
}


[[maybe_unused]] bool ufloat_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot358UFloatDesc[n];
    const char* const desc = d < 0 ? "" : kSlot358MenuStrings[d];
    const std::size_t desc_length = d < 0 ? 0 : kSlot358MenuStringLengths[d];
    const HelperFloatSiteSpec spec{kSlot358UFloatNames[n], kSlot358UFloatNameLengths[n], desc, desc_length, kSlot358VarFlags, kSlot358UFloatFnTag, kSlot358UFloatValueBits[n], kSlot358UFloatEarly[n] != 0};
    return helper_init_unbounded_float_var_site(var, spec, concmd).ok;
}



[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot358BoolNames[n], kSlot358BoolNameLengths[n], "", 0, kSlot358BoolFnTag, concmd, kSlot358BoolValues[n],
                                          kSlot358BoolValues[n]).ok;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot358Result init_slot358_786f50(Slot358Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot358Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!ufloat_var(objects.r01, 0, concmd)) return out;
    ++out.rows_done;   // row 01 ufloat
    if (!uint_var(objects.r02, 0, concmd)) return out;
    ++out.rows_done;   // row 02 uint
    if (!cmd_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 cmd
    if (!family_group_item_26c10(objects.r04, kSlot358MenuStrings[1], kSlot358MenuStringLengths[1], kSlot358MenuStrings[2], kSlot358MenuStringLengths[2], kSlot358ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 04 group
    if (!family_toggle_item_26ea0(objects.r05, kSlot358MenuStrings[2], kSlot358MenuStringLengths[2], kSlot358MenuStrings[3], kSlot358MenuStringLengths[3], target_of(objects.r00), kSlot358ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 05 toge
    if (!family_toggle_item_26ea0(objects.r06, kSlot358MenuStrings[2], kSlot358MenuStringLengths[2], kSlot358MenuStrings[4], kSlot358MenuStringLengths[4], target_of(objects.r01), kSlot358ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 06 toge
    if (!family_toggle_item_26ea0(objects.r07, kSlot358MenuStrings[2], kSlot358MenuStringLengths[2], kSlot358MenuStrings[5], kSlot358MenuStringLengths[5], l2::command_menu_target(objects.r03), kSlot358ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 07 toge
    if (!family_toggle_item_26ea0(objects.r08, kSlot358MenuStrings[2], kSlot358MenuStringLengths[2], kSlot358MenuStrings[6], kSlot358MenuStringLengths[6], unbounded_int_menu_target(objects.r02), kSlot358ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 08 toge
    out.ok = true;
    return out;
}

}  // namespace sotc
