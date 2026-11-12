#include "ctor_slot_429_52e2b0.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot429ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}





// Console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], two f590, __cxa_atexit(0x4b0)); the handler is an identity tag, never called.
[[maybe_unused]] bool cmd_var(ConCmdCommand& command, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot429CmdDesc[n];
    const FConCmdCommandSpec spec{kSlot429CmdNames[n], kSlot429MenuStrings[d], kSlot429MenuStringLengths[d], kSlot429CmdHandler[n]};
    const FConCmdBuildResult built = init_fconcmd_object(command, spec, concmd);
    return built.ok && built.vtable_register_slot_ok && built.views_inert && built.atexit_recorded && !built.handler_invoked;
}






[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot429BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot429BoolNames[n], kSlot429BoolNameLengths[n], d < 0 ? "" : kSlot429MenuStrings[d], d < 0 ? 0 : kSlot429MenuStringLengths[d],
                                          kSlot429BoolFnTag, concmd, kSlot429BoolValues[n], kSlot429BoolValues[n], kSlot429BoolEarly[n] != 0).ok;
}


// Zero pointer slot: the original stores qword 0 and records __cxa_atexit(0x565e80, &slot) (identity tag; the destructor frees the pointee, never run).
bool zero_pointer(std::uint64_t& slot, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {
    slot = 0;
    return concmd.atexit_log.record(dtor, &slot);
}

// Zero-filled container: [+0x08, +0x18) and [+0x18, +0x20) are zeroed, nothing else is written; __cxa_atexit(0x1c5090, container) is recorded (identity tag).
bool zero_container(Slot429Zero& c, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {
    c.q08 = 0;
    c.q10 = 0;
    c.q18 = 0;
    return concmd.atexit_log.record(dtor, &c);
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot429Result init_slot429_52e2b0(Slot429Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot429Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!(objects.external_target_0 != nullptr && family_command_item_26f90(objects.r01, kSlot429MenuStrings[0], kSlot429MenuStringLengths[0], kSlot429MenuStrings[1], kSlot429MenuStringLengths[1], kSlot429MenuStrings[2], kSlot429MenuStringLengths[2], objects.external_target_0, kSlot429ItemFlags, concmd, menu).ok())) return out;
    ++out.rows_done;   // row 01 cmdi
    if (!(objects.external_target_0 != nullptr && family_command_item_26f90(objects.r02, kSlot429MenuStrings[0], kSlot429MenuStringLengths[0], kSlot429MenuStrings[3], kSlot429MenuStringLengths[3], kSlot429MenuStrings[4], kSlot429MenuStringLengths[4], objects.external_target_0, kSlot429ItemFlags, concmd, menu).ok())) return out;
    ++out.rows_done;   // row 02 cmdi
    if (!family_group_item_26c10(objects.r03, kSlot429MenuStrings[0], kSlot429MenuStringLengths[0], kSlot429MenuStrings[5], kSlot429MenuStringLengths[5], kSlot429ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 03 group
    if (!family_group_item_26c10(objects.r04, kSlot429MenuStrings[0], kSlot429MenuStringLengths[0], kSlot429MenuStrings[6], kSlot429MenuStringLengths[6], kSlot429ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 04 group
    if (!family_toggle_item_26ea0(objects.r05, kSlot429MenuStrings[0], kSlot429MenuStringLengths[0], kSlot429MenuStrings[7], kSlot429MenuStringLengths[7], target_of(objects.r00), kSlot429ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 05 toge
    if (!zero_pointer(objects.r06, kSlot429ZeroPtrDtorRva, concmd)) return out;
    ++out.rows_done;   // row 06 zq
    if (!zero_container(objects.r07, kSlot429ZVecDtorRva[0], concmd)) return out;
    ++out.rows_done;   // row 07 zvec
    if (!zero_container(objects.r08, kSlot429ZVecDtorRva[1], concmd)) return out;
    ++out.rows_done;   // row 08 zvec
    if (!zero_container(objects.r09, kSlot429ZVecDtorRva[2], concmd)) return out;
    ++out.rows_done;   // row 09 zvec
    if (!cmd_var(objects.r10, 0, concmd)) return out;
    ++out.rows_done;   // row 10 cmd
    if (!cmd_var(objects.r11, 1, concmd)) return out;
    ++out.rows_done;   // row 11 cmd
    out.ok = true;
    return out;
}


}  // namespace sotc
