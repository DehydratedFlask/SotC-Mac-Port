#include "ctor_slot_424_551140.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot424ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}





// Console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], two f590, __cxa_atexit(0x4b0)); the handler is an identity tag, never called.
[[maybe_unused]] bool cmd_var(ConCmdCommand& command, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot424CmdDesc[n];
    const FConCmdCommandSpec spec{kSlot424CmdNames[n], kSlot424MenuStrings[d], kSlot424MenuStringLengths[d], kSlot424CmdHandler[n]};
    const FConCmdBuildResult built = init_fconcmd_object(command, spec, concmd);
    return built.ok && built.vtable_register_slot_ok && built.views_inert && built.atexit_recorded && !built.handler_invoked;
}








// Store table (batch 18): the straight-line stores of the original, applied from the ELF-tied record table (core/store_table; nothing is executed). The destructor 0x54dbb0 takes the object
// from a fixed address and is recorded as an identity tag with a null object, exactly like the original __cxa_atexit(0x54dbb0, 0, &__dso_handle).
bool store_table_row(Slot424StoreTable& t, ConCmdRegistry& concmd) noexcept {
    const StoreTableSpec spec{kSlot424Stores, kSlot424StoreCount, kSlot424StoreStringBlob, sizeof kSlot424StoreStringBlob, kSlot424StoreStringOffset, kSlot424StoreStringCount};
    if (!replay_store_table(t.bytes, sizeof t.bytes, spec)) return false;
    return concmd.atexit_log.record(kSlot424StoreDtorRva, nullptr);
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot424Result init_slot424_551140(Slot424Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot424Result out{};

    if (!cmd_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 cmd
    if (!cmd_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 cmd
    if (!cmd_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 cmd
    if (!store_table_row(objects.r03, concmd)) return out;
    ++out.rows_done;   // row 03 stab
    out.ok = true;
    return out;
}


}  // namespace sotc
