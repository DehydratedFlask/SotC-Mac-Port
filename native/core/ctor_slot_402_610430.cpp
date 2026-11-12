#include "ctor_slot_402_610430.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot402ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}



// String variable (0x257a0 -> 0x258f0): borrowed name and initial views, then f590(init), f590(name), __cxa_atexit(0x23c90, var).
[[maybe_unused]] bool str_var(ConCmdStringVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const SharedStr name = family_view(kSlot402StrNames[n], kSlot402StrNameLengths[n]);
    const std::int32_t d = kSlot402StrInit[n];
    const SharedStr init = family_view(kSlot402MenuStrings[d], kSlot402MenuStringLengths[d]);
    if (!init_string_var_257a0(var, name, init, concmd).ok) return false;
    if (!release_inert_view_f590(init) || !release_inert_view_f590(name)) return false;
    return concmd.atexit_log.record(kStringVarDtorRva, &var);
}


[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot402FloatNames[n], "", true, kSlot402FloatLoBits[n], kSlot402FloatHiBits[n], kSlot402FloatValueBits[n], kSlot402FloatFnTag, kSlot402VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}





[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot402BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot402BoolNames[n], kSlot402BoolNameLengths[n], d < 0 ? "" : kSlot402MenuStrings[d], d < 0 ? 0 : kSlot402MenuStringLengths[d],
                                          kSlot402BoolFnTag, concmd, kSlot402BoolValues[n], kSlot402BoolValues[n]).ok;
}

[[maybe_unused]] bool enum_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const InlineEnumSpec spec{kSlot402EnumNames[n], kSlot402EnumNameLengths[n], kSlot402VarFlags, kSlot402EnumMidVptr, kSlot402EnumFinalVptr, kSlot402EnumFnTag,
                              kSlot402EnumMin[n], kSlot402EnumMax[n], kSlot402EnumValue[n], kSlot402EnumValue[n], kSlot402EnumDtorRva};
    return init_inline_enum_var(var, spec, concmd).ok;
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

Slot402Result init_slot402_610430(Slot402Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot402Result out{};

    if (!float_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 float
    if (!float_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 float
    if (!enum_var(objects.r02, 0, concmd)) return out;
    ++out.rows_done;   // row 02 ienum
    if (!bool_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 bool
    if (!bool_var(objects.r04, 1, concmd)) return out;
    ++out.rows_done;   // row 04 bool
    if (!str_var(objects.r05, 0, concmd)) return out;
    ++out.rows_done;   // row 05 str
    if (!float_var(objects.r06, 2, concmd)) return out;
    ++out.rows_done;   // row 06 float
    if (!bool_var(objects.r07, 2, concmd)) return out;
    ++out.rows_done;   // row 07 bool
    if (!family_named_item_26db0(objects.r08, kSlot402MenuStrings[1], kSlot402MenuStringLengths[1], objects.r02.base, inline_enum_menu_target(objects.r02, kSlot402EnumFinalVptr, kSlot402EnumSlot3Result, kSlot402EnumSlot6Result), kSlot402ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 08 tog
    if (!family_named_item_26db0(objects.r09, kSlot402MenuStrings[1], kSlot402MenuStringLengths[1], objects.r04.base, target_of(objects.r04), kSlot402ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 09 tog
    if (!slider_item(objects.r10, kSlot402MenuStrings[1], kSlot402MenuStringLengths[1], objects.r06, kSlot402SliderLoBits[0], kSlot402SliderHiBits[0], concmd, menu)) return out;
    ++out.rows_done;   // row 10 slider
    if (!family_named_item_26db0(objects.r11, kSlot402MenuStrings[1], kSlot402MenuStringLengths[1], objects.r03.base, target_of(objects.r03), kSlot402ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 11 tog
    if (!family_named_item_26db0(objects.r12, kSlot402MenuStrings[1], kSlot402MenuStringLengths[1], objects.r07.base, target_of(objects.r07), kSlot402ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 12 tog
    if (!name_holder(objects.r13, kSlot402MenuStrings[2], kSlot402MenuStringLengths[2], kSlot402HolderVptr, kSlot402HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 13 holder
    out.ok = true;
    return out;
}


}  // namespace sotc
