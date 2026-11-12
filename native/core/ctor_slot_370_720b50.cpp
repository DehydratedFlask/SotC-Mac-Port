#include "ctor_slot_370_720b50.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot370ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}



// String variable (0x257a0 -> 0x258f0): borrowed name and initial views, then f590(init), f590(name), __cxa_atexit(0x23c90, var).
[[maybe_unused]] bool str_var(ConCmdStringVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const SharedStr name = family_view(kSlot370StrNames[n], kSlot370StrNameLengths[n]);
    const std::int32_t d = kSlot370StrInit[n];
    const SharedStr init = family_view(kSlot370MenuStrings[d], kSlot370MenuStringLengths[d]);
    if (!init_string_var_257a0(var, name, init, concmd).ok) return false;
    if (!release_inert_view_f590(init) || !release_inert_view_f590(name)) return false;
    return concmd.atexit_log.record(kStringVarDtorRva, &var);
}

// Console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], two f590, __cxa_atexit(0x4b0)); the handler is an identity tag, never called.
[[maybe_unused]] bool cmd_var(ConCmdCommand& command, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot370CmdDesc[n];
    const FConCmdCommandSpec spec{kSlot370CmdNames[n], kSlot370MenuStrings[d], kSlot370MenuStringLengths[d], kSlot370CmdHandler[n]};
    const FConCmdBuildResult built = init_fconcmd_object(command, spec, concmd);
    return built.ok && built.vtable_register_slot_ok && built.views_inert && built.atexit_recorded && !built.handler_invoked;
}

[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot370FloatNames[n], "", true, kSlot370FloatLoBits[n], kSlot370FloatHiBits[n], kSlot370FloatValueBits[n], kSlot370FloatFnTag, kSlot370VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}

[[maybe_unused]] bool ufloat_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot370UFloatDesc[n];
    const char* const desc = d < 0 ? "" : kSlot370MenuStrings[d];
    const std::size_t desc_length = d < 0 ? 0 : kSlot370MenuStringLengths[d];
    const HelperFloatSiteSpec spec{kSlot370UFloatNames[n], kSlot370UFloatNameLengths[n], desc, desc_length, kSlot370VarFlags, kSlot370UFloatFnTag, kSlot370UFloatValueBits[n], kSlot370UFloatEarly[n] != 0};
    return helper_init_unbounded_float_var_site(var, spec, concmd).ok;
}


[[maybe_unused]] bool lint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const SharedStr view = kSlot370LintDesc[n] == nullptr ? empty_string_f730() : family_view(kSlot370LintDesc[n], kSlot370LintDescLength[n]);
    return helper_init_local_int_var_site(var, kSlot370LocalIntSites[n], view, concmd).ok;
}

[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot370BoolNames[n], kSlot370BoolNameLengths[n], "", 0, kSlot370BoolFnTag, concmd, kSlot370BoolValues[n],
                                          kSlot370BoolValues[n]).ok;
}

// `[cell] = name; cell->hash = 0x52a990(name)`; the four bytes at +0x0c are never written.
bool name_cell(NameHashCell& cell, const char* name) noexcept {
    cell.name = name;
    cell.hash = fnv_cstr_52a990(name);
    return true;
}

// The 0x30 byte holder: view copy at +8, zero tail at +0x28, vptr identity tag; the stack view is released by an exact no-op f590 and
// __cxa_atexit(dtor, holder) is recorded (dtor 0x8ef00 or 0x2fced0 per holder class).
bool name_holder(NameHolder30& holder, const char* name, std::size_t length, std::uint64_t vptr, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {
    const SharedStr view = family_view(name, length);
    holder.name = view;
    holder.tail = 0;
    holder.vptr = vptr;
    const bool inert = release_inert_view_f590(view);
    const bool recorded = concmd.atexit_log.record(dtor, &holder);
    return inert && recorded;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot370Result init_slot370_720b50(Slot370Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot370Result out{};

    if (!lint_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 lint
    if (!bool_var(objects.r01, 0, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!bool_var(objects.r02, 1, concmd)) return out;
    ++out.rows_done;   // row 02 bool
    if (!float_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 float
    if (!float_var(objects.r04, 1, concmd)) return out;
    ++out.rows_done;   // row 04 float
    if (!bool_var(objects.r05, 2, concmd)) return out;
    ++out.rows_done;   // row 05 bool
    if (!bool_var(objects.r06, 3, concmd)) return out;
    ++out.rows_done;   // row 06 bool
    if (!float_var(objects.r07, 2, concmd)) return out;
    ++out.rows_done;   // row 07 float
    if (!float_var(objects.r08, 3, concmd)) return out;
    ++out.rows_done;   // row 08 float
    if (!float_var(objects.r09, 4, concmd)) return out;
    ++out.rows_done;   // row 09 float
    if (!float_var(objects.r10, 5, concmd)) return out;
    ++out.rows_done;   // row 10 float
    if (!float_var(objects.r11, 6, concmd)) return out;
    ++out.rows_done;   // row 11 float
    if (!float_var(objects.r12, 7, concmd)) return out;
    ++out.rows_done;   // row 12 float
    if (!float_var(objects.r13, 8, concmd)) return out;
    ++out.rows_done;   // row 13 float
    if (!bool_var(objects.r14, 4, concmd)) return out;
    ++out.rows_done;   // row 14 bool
    if (!float_var(objects.r15, 9, concmd)) return out;
    ++out.rows_done;   // row 15 float
    if (!float_var(objects.r16, 10, concmd)) return out;
    ++out.rows_done;   // row 16 float
    if (!float_var(objects.r17, 11, concmd)) return out;
    ++out.rows_done;   // row 17 float
    if (!float_var(objects.r18, 12, concmd)) return out;
    ++out.rows_done;   // row 18 float
    if (!float_var(objects.r19, 13, concmd)) return out;
    ++out.rows_done;   // row 19 float
    if (!float_var(objects.r20, 14, concmd)) return out;
    ++out.rows_done;   // row 20 float
    if (!float_var(objects.r21, 15, concmd)) return out;
    ++out.rows_done;   // row 21 float
    if (!float_var(objects.r22, 16, concmd)) return out;
    ++out.rows_done;   // row 22 float
    if (!bool_var(objects.r23, 5, concmd)) return out;
    ++out.rows_done;   // row 23 bool
    if (!float_var(objects.r24, 17, concmd)) return out;
    ++out.rows_done;   // row 24 float
    if (!float_var(objects.r25, 18, concmd)) return out;
    ++out.rows_done;   // row 25 float
    if (!float_var(objects.r26, 19, concmd)) return out;
    ++out.rows_done;   // row 26 float
    if (!float_var(objects.r27, 20, concmd)) return out;
    ++out.rows_done;   // row 27 float
    if (!float_var(objects.r28, 21, concmd)) return out;
    ++out.rows_done;   // row 28 float
    if (!float_var(objects.r29, 22, concmd)) return out;
    ++out.rows_done;   // row 29 float
    if (!float_var(objects.r30, 23, concmd)) return out;
    ++out.rows_done;   // row 30 float
    if (!float_var(objects.r31, 24, concmd)) return out;
    ++out.rows_done;   // row 31 float
    if (!bool_var(objects.r32, 6, concmd)) return out;
    ++out.rows_done;   // row 32 bool
    if (!float_var(objects.r33, 25, concmd)) return out;
    ++out.rows_done;   // row 33 float
    if (!float_var(objects.r34, 26, concmd)) return out;
    ++out.rows_done;   // row 34 float
    if (!float_var(objects.r35, 27, concmd)) return out;
    ++out.rows_done;   // row 35 float
    if (!float_var(objects.r36, 28, concmd)) return out;
    ++out.rows_done;   // row 36 float
    if (!float_var(objects.r37, 29, concmd)) return out;
    ++out.rows_done;   // row 37 float
    if (!float_var(objects.r38, 30, concmd)) return out;
    ++out.rows_done;   // row 38 float
    if (!float_var(objects.r39, 31, concmd)) return out;
    ++out.rows_done;   // row 39 float
    if (!float_var(objects.r40, 32, concmd)) return out;
    ++out.rows_done;   // row 40 float
    if (!bool_var(objects.r41, 7, concmd)) return out;
    ++out.rows_done;   // row 41 bool
    if (!bool_var(objects.r42, 8, concmd)) return out;
    ++out.rows_done;   // row 42 bool
    if (!float_var(objects.r43, 33, concmd)) return out;
    ++out.rows_done;   // row 43 float
    if (!float_var(objects.r44, 34, concmd)) return out;
    ++out.rows_done;   // row 44 float
    if (!float_var(objects.r45, 35, concmd)) return out;
    ++out.rows_done;   // row 45 float
    if (!float_var(objects.r46, 36, concmd)) return out;
    ++out.rows_done;   // row 46 float
    if (!float_var(objects.r47, 37, concmd)) return out;
    ++out.rows_done;   // row 47 float
    if (!float_var(objects.r48, 38, concmd)) return out;
    ++out.rows_done;   // row 48 float
    if (!float_var(objects.r49, 39, concmd)) return out;
    ++out.rows_done;   // row 49 float
    if (!bool_var(objects.r50, 9, concmd)) return out;
    ++out.rows_done;   // row 50 bool
    if (!float_var(objects.r51, 40, concmd)) return out;
    ++out.rows_done;   // row 51 float
    if (!float_var(objects.r52, 41, concmd)) return out;
    ++out.rows_done;   // row 52 float
    if (!float_var(objects.r53, 42, concmd)) return out;
    ++out.rows_done;   // row 53 float
    if (!float_var(objects.r54, 43, concmd)) return out;
    ++out.rows_done;   // row 54 float
    if (!float_var(objects.r55, 44, concmd)) return out;
    ++out.rows_done;   // row 55 float
    if (!float_var(objects.r56, 45, concmd)) return out;
    ++out.rows_done;   // row 56 float
    if (!float_var(objects.r57, 46, concmd)) return out;
    ++out.rows_done;   // row 57 float
    if (!bool_var(objects.r58, 10, concmd)) return out;
    ++out.rows_done;   // row 58 bool
    if (!float_var(objects.r59, 47, concmd)) return out;
    ++out.rows_done;   // row 59 float
    if (!float_var(objects.r60, 48, concmd)) return out;
    ++out.rows_done;   // row 60 float
    if (!float_var(objects.r61, 49, concmd)) return out;
    ++out.rows_done;   // row 61 float
    if (!float_var(objects.r62, 50, concmd)) return out;
    ++out.rows_done;   // row 62 float
    if (!float_var(objects.r63, 51, concmd)) return out;
    ++out.rows_done;   // row 63 float
    if (!float_var(objects.r64, 52, concmd)) return out;
    ++out.rows_done;   // row 64 float
    if (!float_var(objects.r65, 53, concmd)) return out;
    ++out.rows_done;   // row 65 float
    if (!bool_var(objects.r66, 11, concmd)) return out;
    ++out.rows_done;   // row 66 bool
    if (!float_var(objects.r67, 54, concmd)) return out;
    ++out.rows_done;   // row 67 float
    if (!float_var(objects.r68, 55, concmd)) return out;
    ++out.rows_done;   // row 68 float
    if (!float_var(objects.r69, 56, concmd)) return out;
    ++out.rows_done;   // row 69 float
    if (!float_var(objects.r70, 57, concmd)) return out;
    ++out.rows_done;   // row 70 float
    if (!float_var(objects.r71, 58, concmd)) return out;
    ++out.rows_done;   // row 71 float
    if (!float_var(objects.r72, 59, concmd)) return out;
    ++out.rows_done;   // row 72 float
    if (!float_var(objects.r73, 60, concmd)) return out;
    ++out.rows_done;   // row 73 float
    if (!bool_var(objects.r74, 12, concmd)) return out;
    ++out.rows_done;   // row 74 bool
    if (!float_var(objects.r75, 61, concmd)) return out;
    ++out.rows_done;   // row 75 float
    if (!float_var(objects.r76, 62, concmd)) return out;
    ++out.rows_done;   // row 76 float
    if (!float_var(objects.r77, 63, concmd)) return out;
    ++out.rows_done;   // row 77 float
    if (!float_var(objects.r78, 64, concmd)) return out;
    ++out.rows_done;   // row 78 float
    if (!float_var(objects.r79, 65, concmd)) return out;
    ++out.rows_done;   // row 79 float
    if (!float_var(objects.r80, 66, concmd)) return out;
    ++out.rows_done;   // row 80 float
    if (!bool_var(objects.r81, 13, concmd)) return out;
    ++out.rows_done;   // row 81 bool
    if (!float_var(objects.r82, 67, concmd)) return out;
    ++out.rows_done;   // row 82 float
    if (!float_var(objects.r83, 68, concmd)) return out;
    ++out.rows_done;   // row 83 float
    if (!float_var(objects.r84, 69, concmd)) return out;
    ++out.rows_done;   // row 84 float
    if (!bool_var(objects.r85, 14, concmd)) return out;
    ++out.rows_done;   // row 85 bool
    if (!bool_var(objects.r86, 15, concmd)) return out;
    ++out.rows_done;   // row 86 bool
    if (!bool_var(objects.r87, 16, concmd)) return out;
    ++out.rows_done;   // row 87 bool
    if (!float_var(objects.r88, 70, concmd)) return out;
    ++out.rows_done;   // row 88 float
    if (!float_var(objects.r89, 71, concmd)) return out;
    ++out.rows_done;   // row 89 float
    if (!float_var(objects.r90, 72, concmd)) return out;
    ++out.rows_done;   // row 90 float
    if (!float_var(objects.r91, 73, concmd)) return out;
    ++out.rows_done;   // row 91 float
    if (!float_var(objects.r92, 74, concmd)) return out;
    ++out.rows_done;   // row 92 float
    if (!float_var(objects.r93, 75, concmd)) return out;
    ++out.rows_done;   // row 93 float
    if (!float_var(objects.r94, 76, concmd)) return out;
    ++out.rows_done;   // row 94 float
    if (!float_var(objects.r95, 77, concmd)) return out;
    ++out.rows_done;   // row 95 float
    if (!float_var(objects.r96, 78, concmd)) return out;
    ++out.rows_done;   // row 96 float
    if (!bool_var(objects.r97, 17, concmd)) return out;
    ++out.rows_done;   // row 97 bool
    if (!ufloat_var(objects.r98, 0, concmd)) return out;
    ++out.rows_done;   // row 98 ufloat
    if (!ufloat_var(objects.r99, 1, concmd)) return out;
    ++out.rows_done;   // row 99 ufloat
    if (!ufloat_var(objects.r100, 2, concmd)) return out;
    ++out.rows_done;   // row 100 ufloat
    if (!float_var(objects.r101, 79, concmd)) return out;
    ++out.rows_done;   // row 101 float
    if (!float_var(objects.r102, 80, concmd)) return out;
    ++out.rows_done;   // row 102 float
    if (!float_var(objects.r103, 81, concmd)) return out;
    ++out.rows_done;   // row 103 float
    if (!float_var(objects.r104, 82, concmd)) return out;
    ++out.rows_done;   // row 104 float
    if (!lint_var(objects.r105, 1, concmd)) return out;
    ++out.rows_done;   // row 105 lint
    if (!float_var(objects.r106, 83, concmd)) return out;
    ++out.rows_done;   // row 106 float
    if (!float_var(objects.r107, 84, concmd)) return out;
    ++out.rows_done;   // row 107 float
    if (!float_var(objects.r108, 85, concmd)) return out;
    ++out.rows_done;   // row 108 float
    if (!bool_var(objects.r109, 18, concmd)) return out;
    ++out.rows_done;   // row 109 bool
    if (!float_var(objects.r110, 86, concmd)) return out;
    ++out.rows_done;   // row 110 float
    if (!bool_var(objects.r111, 19, concmd)) return out;
    ++out.rows_done;   // row 111 bool
    if (!float_var(objects.r112, 87, concmd)) return out;
    ++out.rows_done;   // row 112 float
    if (!float_var(objects.r113, 88, concmd)) return out;
    ++out.rows_done;   // row 113 float
    if (!bool_var(objects.r114, 20, concmd)) return out;
    ++out.rows_done;   // row 114 bool
    if (!float_var(objects.r115, 89, concmd)) return out;
    ++out.rows_done;   // row 115 float
    if (!float_var(objects.r116, 90, concmd)) return out;
    ++out.rows_done;   // row 116 float
    if (!float_var(objects.r117, 91, concmd)) return out;
    ++out.rows_done;   // row 117 float
    if (!bool_var(objects.r118, 21, concmd)) return out;
    ++out.rows_done;   // row 118 bool
    if (!bool_var(objects.r119, 22, concmd)) return out;
    ++out.rows_done;   // row 119 bool
    if (!float_var(objects.r120, 92, concmd)) return out;
    ++out.rows_done;   // row 120 float
    if (!float_var(objects.r121, 93, concmd)) return out;
    ++out.rows_done;   // row 121 float
    if (!bool_var(objects.r122, 23, concmd)) return out;
    ++out.rows_done;   // row 122 bool
    if (!bool_var(objects.r123, 24, concmd)) return out;
    ++out.rows_done;   // row 123 bool
    if (!bool_var(objects.r124, 25, concmd)) return out;
    ++out.rows_done;   // row 124 bool
    if (!float_var(objects.r125, 94, concmd)) return out;
    ++out.rows_done;   // row 125 float
    if (!float_var(objects.r126, 95, concmd)) return out;
    ++out.rows_done;   // row 126 float
    if (!float_var(objects.r127, 96, concmd)) return out;
    ++out.rows_done;   // row 127 float
    if (!float_var(objects.r128, 97, concmd)) return out;
    ++out.rows_done;   // row 128 float
    if (!float_var(objects.r129, 98, concmd)) return out;
    ++out.rows_done;   // row 129 float
    if (!ufloat_var(objects.r130, 3, concmd)) return out;
    ++out.rows_done;   // row 130 ufloat
    if (!ufloat_var(objects.r131, 4, concmd)) return out;
    ++out.rows_done;   // row 131 ufloat
    if (!bool_var(objects.r132, 26, concmd)) return out;
    ++out.rows_done;   // row 132 bool
    if (!bool_var(objects.r133, 27, concmd)) return out;
    ++out.rows_done;   // row 133 bool
    if (!float_var(objects.r134, 99, concmd)) return out;
    ++out.rows_done;   // row 134 float
    if (!float_var(objects.r135, 100, concmd)) return out;
    ++out.rows_done;   // row 135 float
    if (!float_var(objects.r136, 101, concmd)) return out;
    ++out.rows_done;   // row 136 float
    if (!float_var(objects.r137, 102, concmd)) return out;
    ++out.rows_done;   // row 137 float
    if (!bool_var(objects.r138, 28, concmd)) return out;
    ++out.rows_done;   // row 138 bool
    if (!bool_var(objects.r139, 29, concmd)) return out;
    ++out.rows_done;   // row 139 bool
    if (!float_var(objects.r140, 103, concmd)) return out;
    ++out.rows_done;   // row 140 float
    if (!bool_var(objects.r141, 30, concmd)) return out;
    ++out.rows_done;   // row 141 bool
    if (!float_var(objects.r142, 104, concmd)) return out;
    ++out.rows_done;   // row 142 float
    if (!float_var(objects.r143, 105, concmd)) return out;
    ++out.rows_done;   // row 143 float
    if (!float_var(objects.r144, 106, concmd)) return out;
    ++out.rows_done;   // row 144 float
    if (!float_var(objects.r145, 107, concmd)) return out;
    ++out.rows_done;   // row 145 float
    if (!float_var(objects.r146, 108, concmd)) return out;
    ++out.rows_done;   // row 146 float
    if (!float_var(objects.r147, 109, concmd)) return out;
    ++out.rows_done;   // row 147 float
    if (!float_var(objects.r148, 110, concmd)) return out;
    ++out.rows_done;   // row 148 float
    if (!float_var(objects.r149, 111, concmd)) return out;
    ++out.rows_done;   // row 149 float
    if (!float_var(objects.r150, 112, concmd)) return out;
    ++out.rows_done;   // row 150 float
    if (!float_var(objects.r151, 113, concmd)) return out;
    ++out.rows_done;   // row 151 float
    if (!float_var(objects.r152, 114, concmd)) return out;
    ++out.rows_done;   // row 152 float
    if (!float_var(objects.r153, 115, concmd)) return out;
    ++out.rows_done;   // row 153 float
    if (!float_var(objects.r154, 116, concmd)) return out;
    ++out.rows_done;   // row 154 float
    if (!bool_var(objects.r155, 31, concmd)) return out;
    ++out.rows_done;   // row 155 bool
    if (!bool_var(objects.r156, 32, concmd)) return out;
    ++out.rows_done;   // row 156 bool
    if (!bool_var(objects.r157, 33, concmd)) return out;
    ++out.rows_done;   // row 157 bool
    if (!bool_var(objects.r158, 34, concmd)) return out;
    ++out.rows_done;   // row 158 bool
    if (!name_cell(objects.r159, kSlot370MenuStrings[0])) return out;
    ++out.rows_done;   // row 159 cell
    if (!name_cell(objects.r160, kSlot370MenuStrings[1])) return out;
    ++out.rows_done;   // row 160 cell
    if (!family_toggle_item_26ea0(objects.r161, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], kSlot370MenuStrings[3], kSlot370MenuStringLengths[3], target_of(objects.r157), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 161 toge
    if (!family_toggle_item_26ea0(objects.r162, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], kSlot370MenuStrings[4], kSlot370MenuStringLengths[4], target_of(objects.r134), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 162 toge
    if (!family_toggle_item_26ea0(objects.r163, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], kSlot370MenuStrings[5], kSlot370MenuStringLengths[5], target_of(objects.r135), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 163 toge
    if (!family_toggle_item_26ea0(objects.r164, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], kSlot370MenuStrings[6], kSlot370MenuStringLengths[6], target_of(objects.r136), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 164 toge
    if (!family_toggle_item_26ea0(objects.r165, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], kSlot370MenuStrings[7], kSlot370MenuStringLengths[7], target_of(objects.r137), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 165 toge
    if (!family_named_item_26db0(objects.r166, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r139.base, target_of(objects.r139), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 166 tog
    if (!family_named_item_26db0(objects.r167, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r118.base, target_of(objects.r118), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 167 tog
    if (!family_named_item_26db0(objects.r168, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r119.base, target_of(objects.r119), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 168 tog
    if (!family_named_item_26db0(objects.r169, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r120.base, target_of(objects.r120), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 169 tog
    if (!family_named_item_26db0(objects.r170, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r121.base, target_of(objects.r121), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 170 tog
    if (!family_named_item_26db0(objects.r171, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r141.base, target_of(objects.r141), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 171 tog
    if (!family_named_item_26db0(objects.r172, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r122.base, target_of(objects.r122), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 172 tog
    if (!family_named_item_26db0(objects.r173, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r142.base, target_of(objects.r142), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 173 tog
    if (!family_named_item_26db0(objects.r174, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r143.base, target_of(objects.r143), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 174 tog
    if (!family_toggle_item_26ea0(objects.r175, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], kSlot370MenuStrings[8], kSlot370MenuStringLengths[8], local_int_var_menu_target(objects.r105), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 175 toge
    if (!family_named_item_26db0(objects.r176, kSlot370MenuStrings[9], kSlot370MenuStringLengths[9], objects.r152.base, target_of(objects.r152), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 176 tog
    if (!family_named_item_26db0(objects.r177, kSlot370MenuStrings[9], kSlot370MenuStringLengths[9], objects.r153.base, target_of(objects.r153), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 177 tog
    if (!slider_item(objects.r178, kSlot370MenuStrings[9], kSlot370MenuStringLengths[9], objects.r154, kSlot370SliderLoBits[0], kSlot370SliderHiBits[0], concmd, menu)) return out;
    ++out.rows_done;   // row 178 slider
    if (!family_named_item_26db0(objects.r179, kSlot370MenuStrings[9], kSlot370MenuStringLengths[9], objects.r155.base, target_of(objects.r155), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 179 tog
    if (!family_named_item_26db0(objects.r180, kSlot370MenuStrings[10], kSlot370MenuStringLengths[10], objects.r123.base, target_of(objects.r123), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 180 tog
    if (!family_named_item_26db0(objects.r181, kSlot370MenuStrings[10], kSlot370MenuStringLengths[10], objects.r124.base, target_of(objects.r124), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 181 tog
    if (!slider_item(objects.r182, kSlot370MenuStrings[10], kSlot370MenuStringLengths[10], objects.r125, kSlot370SliderLoBits[1], kSlot370SliderHiBits[1], concmd, menu)) return out;
    ++out.rows_done;   // row 182 slider
    if (!slider_item(objects.r183, kSlot370MenuStrings[10], kSlot370MenuStringLengths[10], objects.r126, kSlot370SliderLoBits[2], kSlot370SliderHiBits[2], concmd, menu)) return out;
    ++out.rows_done;   // row 183 slider
    if (!slider_item(objects.r184, kSlot370MenuStrings[10], kSlot370MenuStringLengths[10], objects.r127, kSlot370SliderLoBits[3], kSlot370SliderHiBits[3], concmd, menu)) return out;
    ++out.rows_done;   // row 184 slider
    if (!slider_item(objects.r185, kSlot370MenuStrings[10], kSlot370MenuStringLengths[10], objects.r128, kSlot370SliderLoBits[4], kSlot370SliderHiBits[4], concmd, menu)) return out;
    ++out.rows_done;   // row 185 slider
    if (!slider_item(objects.r186, kSlot370MenuStrings[10], kSlot370MenuStringLengths[10], objects.r129, kSlot370SliderLoBits[5], kSlot370SliderHiBits[5], concmd, menu)) return out;
    ++out.rows_done;   // row 186 slider
    if (!slider_item(objects.r187, kSlot370MenuStrings[11], kSlot370MenuStringLengths[11], objects.r145, kSlot370SliderLoBits[6], kSlot370SliderHiBits[6], concmd, menu)) return out;
    ++out.rows_done;   // row 187 slider
    if (!slider_item(objects.r188, kSlot370MenuStrings[11], kSlot370MenuStringLengths[11], objects.r144, kSlot370SliderLoBits[7], kSlot370SliderHiBits[7], concmd, menu)) return out;
    ++out.rows_done;   // row 188 slider
    if (!slider_item(objects.r189, kSlot370MenuStrings[11], kSlot370MenuStringLengths[11], objects.r146, kSlot370SliderLoBits[8], kSlot370SliderHiBits[8], concmd, menu)) return out;
    ++out.rows_done;   // row 189 slider
    if (!slider_item(objects.r190, kSlot370MenuStrings[11], kSlot370MenuStringLengths[11], objects.r147, kSlot370SliderLoBits[9], kSlot370SliderHiBits[9], concmd, menu)) return out;
    ++out.rows_done;   // row 190 slider
    if (!slider_item(objects.r191, kSlot370MenuStrings[11], kSlot370MenuStringLengths[11], objects.r148, kSlot370SliderLoBits[10], kSlot370SliderHiBits[10], concmd, menu)) return out;
    ++out.rows_done;   // row 191 slider
    if (!slider_item(objects.r192, kSlot370MenuStrings[11], kSlot370MenuStringLengths[11], objects.r149, kSlot370SliderLoBits[11], kSlot370SliderHiBits[11], concmd, menu)) return out;
    ++out.rows_done;   // row 192 slider
    if (!slider_item(objects.r193, kSlot370MenuStrings[11], kSlot370MenuStringLengths[11], objects.r150, kSlot370SliderLoBits[12], kSlot370SliderHiBits[12], concmd, menu)) return out;
    ++out.rows_done;   // row 193 slider
    if (!slider_item(objects.r194, kSlot370MenuStrings[11], kSlot370MenuStringLengths[11], objects.r151, kSlot370SliderLoBits[13], kSlot370SliderHiBits[13], concmd, menu)) return out;
    ++out.rows_done;   // row 194 slider
    if (!family_named_item_26db0(objects.r195, kSlot370MenuStrings[12], kSlot370MenuStringLengths[12], objects.r156.base, target_of(objects.r156), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 195 tog
    if (!family_named_item_26db0(objects.r196, kSlot370MenuStrings[13], kSlot370MenuStringLengths[13], objects.r158.base, target_of(objects.r158), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 196 tog
    if (!bool_var(objects.r197, 35, concmd)) return out;
    ++out.rows_done;   // row 197 bool
    if (!name_cell(objects.r198, kSlot370MenuStrings[14])) return out;
    ++out.rows_done;   // row 198 cell
    if (!name_cell(objects.r199, kSlot370MenuStrings[15])) return out;
    ++out.rows_done;   // row 199 cell
    if (!cmd_var(objects.r200, 0, concmd)) return out;
    ++out.rows_done;   // row 200 cmd
    if (!name_cell(objects.r201, kSlot370MenuStrings[17])) return out;
    ++out.rows_done;   // row 201 cell
    if (!cmd_var(objects.r202, 1, concmd)) return out;
    ++out.rows_done;   // row 202 cmd
    if (!cmd_var(objects.r203, 2, concmd)) return out;
    ++out.rows_done;   // row 203 cmd
    if (!str_var(objects.r204, 0, concmd)) return out;
    ++out.rows_done;   // row 204 str
    if (!name_holder(objects.r205, kSlot370MenuStrings[21], kSlot370MenuStringLengths[21], kSlot370HolderVptr, kSlot370HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 205 holder
    if (!float_var(objects.r206, 117, concmd)) return out;
    ++out.rows_done;   // row 206 float
    if (!float_var(objects.r207, 118, concmd)) return out;
    ++out.rows_done;   // row 207 float
    if (!float_var(objects.r208, 119, concmd)) return out;
    ++out.rows_done;   // row 208 float
    if (!float_var(objects.r209, 120, concmd)) return out;
    ++out.rows_done;   // row 209 float
    if (!float_var(objects.r210, 121, concmd)) return out;
    ++out.rows_done;   // row 210 float
    if (!float_var(objects.r211, 122, concmd)) return out;
    ++out.rows_done;   // row 211 float
    if (!float_var(objects.r212, 123, concmd)) return out;
    ++out.rows_done;   // row 212 float
    if (!family_named_item_26db0(objects.r213, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r206.base, target_of(objects.r206), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 213 tog
    if (!family_named_item_26db0(objects.r214, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r207.base, target_of(objects.r207), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 214 tog
    if (!family_named_item_26db0(objects.r215, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r208.base, target_of(objects.r208), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 215 tog
    if (!family_named_item_26db0(objects.r216, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r209.base, target_of(objects.r209), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 216 tog
    if (!family_named_item_26db0(objects.r217, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r210.base, target_of(objects.r210), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 217 tog
    if (!family_named_item_26db0(objects.r218, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r211.base, target_of(objects.r211), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 218 tog
    if (!family_named_item_26db0(objects.r219, kSlot370MenuStrings[2], kSlot370MenuStringLengths[2], objects.r212.base, target_of(objects.r212), kSlot370ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 219 tog
    if (!name_cell(objects.r220, kSlot370MenuStrings[22])) return out;
    ++out.rows_done;   // row 220 cell
    if (!name_cell(objects.r221, kSlot370MenuStrings[23])) return out;
    ++out.rows_done;   // row 221 cell
    if (!name_cell(objects.r222, kSlot370MenuStrings[24])) return out;
    ++out.rows_done;   // row 222 cell
    if (!name_cell(objects.r223, kSlot370MenuStrings[25])) return out;
    ++out.rows_done;   // row 223 cell
    if (!name_cell(objects.r224, kSlot370MenuStrings[26])) return out;
    ++out.rows_done;   // row 224 cell
    if (!float_var(objects.r225, 124, concmd)) return out;
    ++out.rows_done;   // row 225 float
    if (!float_var(objects.r226, 125, concmd)) return out;
    ++out.rows_done;   // row 226 float
    if (!float_var(objects.r227, 126, concmd)) return out;
    ++out.rows_done;   // row 227 float
    if (!float_var(objects.r228, 127, concmd)) return out;
    ++out.rows_done;   // row 228 float
    if (!float_var(objects.r229, 128, concmd)) return out;
    ++out.rows_done;   // row 229 float
    if (!float_var(objects.r230, 129, concmd)) return out;
    ++out.rows_done;   // row 230 float
    if (!float_var(objects.r231, 130, concmd)) return out;
    ++out.rows_done;   // row 231 float
    out.ok = true;
    return out;
}


}  // namespace sotc
