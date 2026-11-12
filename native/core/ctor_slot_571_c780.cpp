#include "ctor_slot_571_c780.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot571ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}




// Console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], two f590, __cxa_atexit(0x4b0)); the handler is an identity tag, never called.
[[maybe_unused]] bool cmd_var(ConCmdCommand& command, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot571CmdDesc[n];
    const FConCmdCommandSpec spec{kSlot571CmdNames[n], kSlot571MenuStrings[d], kSlot571MenuStringLengths[d], kSlot571CmdHandler[n]};
    const FConCmdBuildResult built = init_fconcmd_object(command, spec, concmd);
    return built.ok && built.vtable_register_slot_ok && built.views_inert && built.atexit_recorded && !built.handler_invoked;
}

[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot571FloatNames[n], "", true, kSlot571FloatLoBits[n], kSlot571FloatHiBits[n], kSlot571FloatValueBits[n], kSlot571FloatFnTag, kSlot571VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}





[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot571BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot571BoolNames[n], kSlot571BoolNameLengths[n], d < 0 ? "" : kSlot571MenuStrings[d], d < 0 ? 0 : kSlot571MenuStringLengths[d],
                                          kSlot571BoolFnTag, concmd, kSlot571BoolValues[n], kSlot571BoolValues[n]).ok;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot571Result init_slot571_c780(Slot571Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot571Result out{};

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
    if (!bool_var(objects.r07, 7, concmd)) return out;
    ++out.rows_done;   // row 07 bool
    if (!bool_var(objects.r08, 8, concmd)) return out;
    ++out.rows_done;   // row 08 bool
    if (!bool_var(objects.r09, 9, concmd)) return out;
    ++out.rows_done;   // row 09 bool
    if (!bool_var(objects.r10, 10, concmd)) return out;
    ++out.rows_done;   // row 10 bool
    if (!bool_var(objects.r11, 11, concmd)) return out;
    ++out.rows_done;   // row 11 bool
    if (!bool_var(objects.r12, 12, concmd)) return out;
    ++out.rows_done;   // row 12 bool
    if (!bool_var(objects.r13, 13, concmd)) return out;
    ++out.rows_done;   // row 13 bool
    if (!bool_var(objects.r14, 14, concmd)) return out;
    ++out.rows_done;   // row 14 bool
    if (!bool_var(objects.r15, 15, concmd)) return out;
    ++out.rows_done;   // row 15 bool
    if (!bool_var(objects.r16, 16, concmd)) return out;
    ++out.rows_done;   // row 16 bool
    if (!bool_var(objects.r17, 17, concmd)) return out;
    ++out.rows_done;   // row 17 bool
    if (!bool_var(objects.r18, 18, concmd)) return out;
    ++out.rows_done;   // row 18 bool
    if (!bool_var(objects.r19, 19, concmd)) return out;
    ++out.rows_done;   // row 19 bool
    if (!float_var(objects.r20, 0, concmd)) return out;
    ++out.rows_done;   // row 20 float
    if (!float_var(objects.r21, 1, concmd)) return out;
    ++out.rows_done;   // row 21 float
    if (!float_var(objects.r22, 2, concmd)) return out;
    ++out.rows_done;   // row 22 float
    if (!float_var(objects.r23, 3, concmd)) return out;
    ++out.rows_done;   // row 23 float
    if (!float_var(objects.r24, 4, concmd)) return out;
    ++out.rows_done;   // row 24 float
    if (!float_var(objects.r25, 5, concmd)) return out;
    ++out.rows_done;   // row 25 float
    if (!float_var(objects.r26, 6, concmd)) return out;
    ++out.rows_done;   // row 26 float
    if (!float_var(objects.r27, 7, concmd)) return out;
    ++out.rows_done;   // row 27 float
    if (!float_var(objects.r28, 8, concmd)) return out;
    ++out.rows_done;   // row 28 float
    if (!float_var(objects.r29, 9, concmd)) return out;
    ++out.rows_done;   // row 29 float
    if (!float_var(objects.r30, 10, concmd)) return out;
    ++out.rows_done;   // row 30 float
    if (!bool_var(objects.r31, 20, concmd)) return out;
    ++out.rows_done;   // row 31 bool
    if (!bool_var(objects.r32, 21, concmd)) return out;
    ++out.rows_done;   // row 32 bool
    if (!float_var(objects.r33, 11, concmd)) return out;
    ++out.rows_done;   // row 33 float
    if (!float_var(objects.r34, 12, concmd)) return out;
    ++out.rows_done;   // row 34 float
    if (!bool_var(objects.r35, 22, concmd)) return out;
    ++out.rows_done;   // row 35 bool
    if (!bool_var(objects.r36, 23, concmd)) return out;
    ++out.rows_done;   // row 36 bool
    if (!bool_var(objects.r37, 24, concmd)) return out;
    ++out.rows_done;   // row 37 bool
    if (!bool_var(objects.r38, 25, concmd)) return out;
    ++out.rows_done;   // row 38 bool
    if (!cmd_var(objects.r39, 0, concmd)) return out;
    ++out.rows_done;   // row 39 cmd
    if (!cmd_var(objects.r40, 1, concmd)) return out;
    ++out.rows_done;   // row 40 cmd
    if (!cmd_var(objects.r41, 2, concmd)) return out;
    ++out.rows_done;   // row 41 cmd
    if (!cmd_var(objects.r42, 3, concmd)) return out;
    ++out.rows_done;   // row 42 cmd
    if (!(objects.external_target_0 != nullptr && family_command_item_26f90(objects.r43, kSlot571MenuStrings[4], kSlot571MenuStringLengths[4], kSlot571MenuStrings[5], kSlot571MenuStringLengths[5], kSlot571MenuStrings[6], kSlot571MenuStringLengths[6], objects.external_target_0, kSlot571ItemFlags, concmd, menu).ok())) return out;
    ++out.rows_done;   // row 43 cmdi
    out.ok = true;
    return out;
}


}  // namespace sotc
