#include "ctor_slot_572_1a20.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot572ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}




// Console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], two f590, __cxa_atexit(0x4b0)); the handler is an identity tag, never called.
[[maybe_unused]] bool cmd_var(ConCmdCommand& command, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot572CmdDesc[n];
    const FConCmdCommandSpec spec{kSlot572CmdNames[n], kSlot572MenuStrings[d], kSlot572MenuStringLengths[d], kSlot572CmdHandler[n]};
    const FConCmdBuildResult built = init_fconcmd_object(command, spec, concmd);
    return built.ok && built.vtable_register_slot_ok && built.views_inert && built.atexit_recorded && !built.handler_invoked;
}







[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot572Result init_slot572_1a20(Slot572Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot572Result out{};

    if (!cmd_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 cmd
    if (!cmd_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 cmd
    if (!cmd_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 cmd
    if (!cmd_var(objects.r03, 3, concmd)) return out;
    ++out.rows_done;   // row 03 cmd
    if (!cmd_var(objects.r04, 4, concmd)) return out;
    ++out.rows_done;   // row 04 cmd
    if (!family_toggle_item_26ea0(objects.r05, kSlot572MenuStrings[4], kSlot572MenuStringLengths[4], kSlot572MenuStrings[5], kSlot572MenuStringLengths[5], l2::command_menu_target(objects.r02), kSlot572ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 05 toge
    if (!family_toggle_item_26ea0(objects.r06, kSlot572MenuStrings[4], kSlot572MenuStringLengths[4], kSlot572MenuStrings[6], kSlot572MenuStringLengths[6], l2::command_menu_target(objects.r04), kSlot572ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 06 toge
    if (!(objects.external_target_0 != nullptr && family_command_item_26f90(objects.r07, kSlot572MenuStrings[4], kSlot572MenuStringLengths[4], kSlot572MenuStrings[7], kSlot572MenuStringLengths[7], kSlot572MenuStrings[8], kSlot572MenuStringLengths[8], objects.external_target_0, kSlot572ItemFlags, concmd, menu).ok())) return out;
    ++out.rows_done;   // row 07 cmdi
    out.ok = true;
    return out;
}


}  // namespace sotc
