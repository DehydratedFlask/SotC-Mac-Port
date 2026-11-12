#include "ctor_slot_391_68ebc0.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot391ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}



// Console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], two f590, __cxa_atexit(0x4b0)); the handler is an identity tag, never called.
[[maybe_unused]] bool cmd_var(ConCmdCommand& command, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot391CmdDesc[n];
    const FConCmdCommandSpec spec{kSlot391CmdNames[n], kSlot391MenuStrings[d], kSlot391MenuStringLengths[d], kSlot391CmdHandler[n]};
    const FConCmdBuildResult built = init_fconcmd_object(command, spec, concmd);
    return built.ok && built.vtable_register_slot_ok && built.views_inert && built.atexit_recorded && !built.handler_invoked;
}





[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot391BoolNames[n], kSlot391BoolNameLengths[n], "", 0, kSlot391BoolFnTag, concmd, kSlot391BoolValues[n],
                                          kSlot391BoolValues[n]).ok;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot391Result init_slot391_68ebc0(Slot391Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot391Result out{};

    if (!cmd_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 cmd
    if (!cmd_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 cmd
    if (!bool_var(objects.r02, 0, concmd)) return out;
    ++out.rows_done;   // row 02 bool
    if (!family_toggle_item_26ea0(objects.r03, kSlot391MenuStrings[2], kSlot391MenuStringLengths[2], kSlot391MenuStrings[3], kSlot391MenuStringLengths[3], target_of(objects.r02), kSlot391ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 03 toge
    out.ok = true;
    return out;
}

}  // namespace sotc
