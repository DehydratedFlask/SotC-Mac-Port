#include "ctor_slot_473_355a00.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot473ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}




// String variable (0x257a0 -> 0x258f0): borrowed name and initial views, then f590(init), f590(name), __cxa_atexit(0x23c90, var).
[[maybe_unused]] bool str_var(ConCmdStringVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const SharedStr name = family_view(kSlot473StrNames[n], kSlot473StrNameLengths[n]);
    const std::int32_t d = kSlot473StrInit[n];
    const SharedStr init = family_view(kSlot473MenuStrings[d], kSlot473MenuStringLengths[d]);
    if (!init_string_var_257a0(var, name, init, concmd).ok) return false;
    if (!release_inert_view_f590(init) || !release_inert_view_f590(name)) return false;
    return concmd.atexit_log.record(kStringVarDtorRva, &var);
}

// Console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], two f590, __cxa_atexit(0x4b0)); the handler is an identity tag, never called.
[[maybe_unused]] bool cmd_var(ConCmdCommand& command, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot473CmdDesc[n];
    const FConCmdCommandSpec spec{kSlot473CmdNames[n], kSlot473MenuStrings[d], kSlot473MenuStringLengths[d], kSlot473CmdHandler[n]};
    const FConCmdBuildResult built = init_fconcmd_object(command, spec, concmd);
    return built.ok && built.vtable_register_slot_ok && built.views_inert && built.atexit_recorded && !built.handler_invoked;
}


[[maybe_unused]] bool ufloat_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot473UFloatDesc[n];
    const char* const desc = d < 0 ? "" : kSlot473MenuStrings[d];
    const std::size_t desc_length = d < 0 ? 0 : kSlot473MenuStringLengths[d];
    const HelperFloatSiteSpec spec{kSlot473UFloatNames[n], kSlot473UFloatNameLengths[n], desc, desc_length, kSlot473VarFlags, kSlot473UFloatFnTag, kSlot473UFloatValueBits[n], kSlot473UFloatEarly[n] != 0};
    return helper_init_unbounded_float_var_site(var, spec, concmd).ok;
}




[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot473BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot473BoolNames[n], kSlot473BoolNameLengths[n], d < 0 ? "" : kSlot473MenuStrings[d], d < 0 ? 0 : kSlot473MenuStringLengths[d],
                                          kSlot473BoolFnTag, concmd, kSlot473BoolValues[n], kSlot473BoolValues[n], kSlot473BoolEarly[n] != 0).ok;
}

[[maybe_unused]] bool boolx_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const BoolVar274730SiteResult r = helper_init_bool_var_274730_site(var, kSlot473BoolXSites[n], concmd);   // 0x274730 ; call [argfn + 0x18] ; f590(name) ; __cxa_atexit(0x20b0)
    return r.ok && r.refusal == HelperRefusal::none;
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

// Ring / vector header: helper_1f220_ring_header (four qword stores), then __cxa_atexit(0x354470, header) is recorded (identity tag; the destructor frees the buffer, never run).
bool ring_header(Slot473Ring& c, ConCmdRegistry& concmd) noexcept {
    ring_header_init_1f220(&c);
    return concmd.atexit_log.record(kSlot473RingDtorRva, &c);
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot473Result init_slot473_355a00(Slot473Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot473Result out{};

    if (!name_holder(objects.r00, kSlot473MenuStrings[0], kSlot473MenuStringLengths[0], kSlot473HolderVptr, kSlot473HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 00 holder
    if (!bool_var(objects.r01, 0, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!bool_var(objects.r02, 1, concmd)) return out;
    ++out.rows_done;   // row 02 bool
    if (!boolx_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 boolx
    if (!boolx_var(objects.r04, 1, concmd)) return out;
    ++out.rows_done;   // row 04 boolx
    if (!bool_var(objects.r05, 2, concmd)) return out;
    ++out.rows_done;   // row 05 bool
    if (!bool_var(objects.r06, 3, concmd)) return out;
    ++out.rows_done;   // row 06 bool
    if (!boolx_var(objects.r07, 2, concmd)) return out;
    ++out.rows_done;   // row 07 boolx
    if (!boolx_var(objects.r08, 3, concmd)) return out;
    ++out.rows_done;   // row 08 boolx
    if (!boolx_var(objects.r09, 4, concmd)) return out;
    ++out.rows_done;   // row 09 boolx
    if (!boolx_var(objects.r10, 5, concmd)) return out;
    ++out.rows_done;   // row 10 boolx
    if (!boolx_var(objects.r11, 6, concmd)) return out;
    ++out.rows_done;   // row 11 boolx
    if (!boolx_var(objects.r12, 7, concmd)) return out;
    ++out.rows_done;   // row 12 boolx
    if (!boolx_var(objects.r13, 8, concmd)) return out;
    ++out.rows_done;   // row 13 boolx
    if (!bool_var(objects.r14, 4, concmd)) return out;
    ++out.rows_done;   // row 14 bool
    if (!ufloat_var(objects.r15, 0, concmd)) return out;
    ++out.rows_done;   // row 15 ufloat
    if (!ufloat_var(objects.r16, 1, concmd)) return out;
    ++out.rows_done;   // row 16 ufloat
    if (!family_group_item_26c10(objects.r17, kSlot473MenuStrings[1], kSlot473MenuStringLengths[1], kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 17 group
    if (!family_toggle_item_26ea0(objects.r18, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[3], kSlot473MenuStringLengths[3], target_of(objects.r01), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 18 toge
    if (!family_toggle_item_26ea0(objects.r19, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[4], kSlot473MenuStringLengths[4], target_of(objects.r02), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 19 toge
    if (!family_toggle_item_26ea0(objects.r20, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[5], kSlot473MenuStringLengths[5], target_of(objects.r14), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 20 toge
    if (!family_separator_item_26b60(objects.r21, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473ItemFlags, objects.separator_alloc, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 21 sep
    if (!family_toggle_item_26ea0(objects.r22, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[6], kSlot473MenuStringLengths[6], target_of(objects.r05), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 22 toge
    if (!family_toggle_item_26ea0(objects.r23, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[7], kSlot473MenuStringLengths[7], target_of(objects.r06), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 23 toge
    if (!family_toggle_item_26ea0(objects.r24, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[8], kSlot473MenuStringLengths[8], target_of(objects.r07), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 24 toge
    if (!family_toggle_item_26ea0(objects.r25, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[9], kSlot473MenuStringLengths[9], target_of(objects.r08), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 25 toge
    if (!family_separator_item_26b60(objects.r26, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473ItemFlags, objects.separator_alloc, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 26 sep
    if (!family_toggle_item_26ea0(objects.r27, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[10], kSlot473MenuStringLengths[10], target_of(objects.r09), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 27 toge
    if (!family_toggle_item_26ea0(objects.r28, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[11], kSlot473MenuStringLengths[11], target_of(objects.r10), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 28 toge
    if (!family_toggle_item_26ea0(objects.r29, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[12], kSlot473MenuStringLengths[12], target_of(objects.r12), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 29 toge
    if (!family_toggle_item_26ea0(objects.r30, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[13], kSlot473MenuStringLengths[13], target_of(objects.r11), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 30 toge
    if (!family_toggle_item_26ea0(objects.r31, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[14], kSlot473MenuStringLengths[14], target_of(objects.r13), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 31 toge
    if (!family_separator_item_26b60(objects.r32, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473ItemFlags, objects.separator_alloc, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 32 sep
    if (!family_toggle_item_26ea0(objects.r33, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[15], kSlot473MenuStringLengths[15], target_of(objects.r03), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 33 toge
    if (!family_toggle_item_26ea0(objects.r34, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[16], kSlot473MenuStringLengths[16], target_of(objects.r04), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 34 toge
    if (!family_separator_item_26b60(objects.r35, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473ItemFlags, objects.separator_alloc, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 35 sep
    if (!family_toggle_item_26ea0(objects.r36, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[17], kSlot473MenuStringLengths[17], target_of(objects.r15), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 36 toge
    if (!family_toggle_item_26ea0(objects.r37, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473MenuStrings[18], kSlot473MenuStringLengths[18], target_of(objects.r16), kSlot473ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 37 toge
    if (!family_separator_item_26b60(objects.r38, kSlot473MenuStrings[2], kSlot473MenuStringLengths[2], kSlot473ItemFlags, objects.separator_alloc, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 38 sep
    if (!ring_header(objects.r39, concmd)) return out;
    ++out.rows_done;   // row 39 ringhdr
    if (!str_var(objects.r40, 0, concmd)) return out;
    ++out.rows_done;   // row 40 str
    if (!cmd_var(objects.r41, 0, concmd)) return out;
    ++out.rows_done;   // row 41 cmd
    out.ok = true;
    return out;
}


}  // namespace sotc
