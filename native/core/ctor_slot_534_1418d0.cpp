#include "ctor_slot_534_1418d0.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot534ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}


[[maybe_unused]] bool uint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    if (kSlot534UIntTwin[n] != 0) {
        const HelperIntTwinSiteSpec twin{kSlot534UIntNames[n], kSlot534UIntNameLengths[n], "", 0, kSlot534VarFlags, kSlot534UIntValue[n], kSlot534UIntValue[n], kSlot534UIntTwinFinalVptr, kSlot534UIntTwinDtorRva, kSlot534UIntEarly[n] != 0};
        return helper_init_unbounded_int_twin_var_site(var, UnboundedIntTwinVariant::a_140b00, twin, nullptr, concmd).ok;
    }
    const HelperIntSiteSpec spec{kSlot534UIntNames[n], kSlot534UIntNameLengths[n], "", 0, 0, 0, kSlot534VarFlags, kSlot534UIntFnTag, kSlot534UIntValue[n], kSlot534UIntValue[n], kSlot534UIntEarly[n] != 0};
    return helper_init_unbounded_int_var_site(var, spec, concmd).ok;
}



// Console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], two f590, __cxa_atexit(0x4b0)); the handler is an identity tag, never called.
[[maybe_unused]] bool cmd_var(ConCmdCommand& command, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot534CmdDesc[n];
    const FConCmdCommandSpec spec{kSlot534CmdNames[n], kSlot534MenuStrings[d], kSlot534MenuStringLengths[d], kSlot534CmdHandler[n]};
    const FConCmdBuildResult built = init_fconcmd_object(command, spec, concmd);
    return built.ok && built.vtable_register_slot_ok && built.views_inert && built.atexit_recorded && !built.handler_invoked;
}


[[maybe_unused]] bool ufloat_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot534UFloatDesc[n];
    const char* const desc = d < 0 ? "" : kSlot534MenuStrings[d];
    const std::size_t desc_length = d < 0 ? 0 : kSlot534MenuStringLengths[d];
    const HelperFloatSiteSpec spec{kSlot534UFloatNames[n], kSlot534UFloatNameLengths[n], desc, desc_length, kSlot534VarFlags, kSlot534UFloatFnTag, kSlot534UFloatValueBits[n], kSlot534UFloatEarly[n] != 0};
    return helper_init_unbounded_float_var_site(var, spec, concmd).ok;
}


[[maybe_unused]] bool int_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const HelperIntSiteSpec spec{kSlot534IntNames[n], kSlot534IntNameLengths[n], "", 0, kSlot534IntMin[n], kSlot534IntMax[n], kSlot534VarFlags, kSlot534IntFnTag, kSlot534IntValue[n], kSlot534IntValue[n]};
    return helper_init_ranged_int_var_site(var, spec, concmd).ok;
}


[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot534BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot534BoolNames[n], kSlot534BoolNameLengths[n], d < 0 ? "" : kSlot534MenuStrings[d], d < 0 ? 0 : kSlot534MenuStringLengths[d],
                                          kSlot534BoolFnTag, concmd, kSlot534BoolValues[n], kSlot534BoolValues[n], kSlot534BoolEarly[n] != 0).ok;
}


[[maybe_unused]] bool enum_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const InlineEnumSpec spec{kSlot534EnumNames[n], kSlot534EnumNameLengths[n], kSlot534VarFlags, kSlot534EnumMidVptr, kSlot534EnumFinalVptr, kSlot534EnumFnTag,
                              kSlot534EnumMin[n], kSlot534EnumMax[n], kSlot534EnumValue[n], kSlot534EnumValue[n], kSlot534EnumDtorRva, kSlot534EnumBounded};
    return init_inline_enum_var(var, spec, concmd).ok;
}

// Zero cell: the qword at +0 is zeroed, nothing else is written; __cxa_atexit(0x13a0d0, cell) is recorded (identity tag; the destructor deletes through [+0] and clears the cell, never run).
bool zero_cell(Slot534ZeroCell& c, ConCmdRegistry& concmd) noexcept {
    c.q00 = 0;
    return concmd.atexit_log.record(kSlot534ZeroCellDtorRva, &c);
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot534Result init_slot534_1418d0(Slot534Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot534Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!bool_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!uint_var(objects.r02, 0, concmd)) return out;
    ++out.rows_done;   // row 02 uint
    if (!uint_var(objects.r03, 1, concmd)) return out;
    ++out.rows_done;   // row 03 uint
    if (!uint_var(objects.r04, 2, concmd)) return out;
    ++out.rows_done;   // row 04 uint
    if (!bool_var(objects.r05, 2, concmd)) return out;
    ++out.rows_done;   // row 05 bool
    if (!bool_var(objects.r06, 3, concmd)) return out;
    ++out.rows_done;   // row 06 bool
    if (!ufloat_var(objects.r07, 0, concmd)) return out;
    ++out.rows_done;   // row 07 ufloat
    if (!ufloat_var(objects.r08, 1, concmd)) return out;
    ++out.rows_done;   // row 08 ufloat
    if (!uint_var(objects.r09, 3, concmd)) return out;
    ++out.rows_done;   // row 09 uint
    if (!uint_var(objects.r10, 4, concmd)) return out;
    ++out.rows_done;   // row 10 uint
    if (!bool_var(objects.r11, 4, concmd)) return out;
    ++out.rows_done;   // row 11 bool
    if (!bool_var(objects.r12, 5, concmd)) return out;
    ++out.rows_done;   // row 12 bool
    if (!int_var(objects.r13, 0, concmd)) return out;
    ++out.rows_done;   // row 13 int
    if (!uint_var(objects.r14, 5, concmd)) return out;
    ++out.rows_done;   // row 14 uint
    if (!uint_var(objects.r15, 6, concmd)) return out;
    ++out.rows_done;   // row 15 uint
    if (!uint_var(objects.r16, 7, concmd)) return out;
    ++out.rows_done;   // row 16 uint
    if (!uint_var(objects.r17, 8, concmd)) return out;
    ++out.rows_done;   // row 17 uint
    if (!uint_var(objects.r18, 9, concmd)) return out;
    ++out.rows_done;   // row 18 uint
    if (!uint_var(objects.r19, 10, concmd)) return out;
    ++out.rows_done;   // row 19 uint
    if (!enum_var(objects.r20, 0, concmd)) return out;
    ++out.rows_done;   // row 20 ienum
    if (!zero_cell(objects.r21, concmd)) return out;
    ++out.rows_done;   // row 21 zcell
    if (!cmd_var(objects.r22, 0, concmd)) return out;
    ++out.rows_done;   // row 22 cmd
    if (!cmd_var(objects.r23, 1, concmd)) return out;
    ++out.rows_done;   // row 23 cmd
    out.ok = true;
    return out;
}


}  // namespace sotc
