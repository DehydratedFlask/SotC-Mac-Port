#include "ctor_slot_349_7bde50.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot349ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}

[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot349FloatNames[n], "", true, kSlot349FloatLoBits[n], kSlot349FloatHiBits[n], kSlot349FloatValueBits[n], kSlot349FloatFnTag, kSlot349VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}


[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot349BoolNames[n], kSlot349BoolNameLengths[n], "", 0, kSlot349BoolFnTag, concmd, kSlot349BoolValues[n],
                                          kSlot349BoolValues[n]).ok;
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

Slot349Result init_slot349_7bde50(Slot349Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot349Result out{};

    if (!float_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 float
    if (!float_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 float
    if (!float_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 float
    if (!float_var(objects.r03, 3, concmd)) return out;
    ++out.rows_done;   // row 03 float
    if (!float_var(objects.r04, 4, concmd)) return out;
    ++out.rows_done;   // row 04 float
    if (!float_var(objects.r05, 5, concmd)) return out;
    ++out.rows_done;   // row 05 float
    if (!float_var(objects.r06, 6, concmd)) return out;
    ++out.rows_done;   // row 06 float
    if (!bool_var(objects.r07, 0, concmd)) return out;
    ++out.rows_done;   // row 07 bool
    if (!float_var(objects.r08, 7, concmd)) return out;
    ++out.rows_done;   // row 08 float
    if (!float_var(objects.r09, 8, concmd)) return out;
    ++out.rows_done;   // row 09 float
    if (!float_var(objects.r10, 9, concmd)) return out;
    ++out.rows_done;   // row 10 float
    if (!float_var(objects.r11, 10, concmd)) return out;
    ++out.rows_done;   // row 11 float
    if (!float_var(objects.r12, 11, concmd)) return out;
    ++out.rows_done;   // row 12 float
    if (!float_var(objects.r13, 12, concmd)) return out;
    ++out.rows_done;   // row 13 float
    if (!float_var(objects.r14, 13, concmd)) return out;
    ++out.rows_done;   // row 14 float
    if (!float_var(objects.r15, 14, concmd)) return out;
    ++out.rows_done;   // row 15 float
    if (!float_var(objects.r16, 15, concmd)) return out;
    ++out.rows_done;   // row 16 float
    if (!float_var(objects.r17, 16, concmd)) return out;
    ++out.rows_done;   // row 17 float
    if (!float_var(objects.r18, 17, concmd)) return out;
    ++out.rows_done;   // row 18 float
    if (!float_var(objects.r19, 18, concmd)) return out;
    ++out.rows_done;   // row 19 float
    if (!float_var(objects.r20, 19, concmd)) return out;
    ++out.rows_done;   // row 20 float
    if (!float_var(objects.r21, 20, concmd)) return out;
    ++out.rows_done;   // row 21 float
    if (!float_var(objects.r22, 21, concmd)) return out;
    ++out.rows_done;   // row 22 float
    if (!bool_var(objects.r23, 1, concmd)) return out;
    ++out.rows_done;   // row 23 bool
    if (!bool_var(objects.r24, 2, concmd)) return out;
    ++out.rows_done;   // row 24 bool
    if (!bool_var(objects.r25, 3, concmd)) return out;
    ++out.rows_done;   // row 25 bool
    if (!float_var(objects.r26, 22, concmd)) return out;
    ++out.rows_done;   // row 26 float
    if (!bool_var(objects.r27, 4, concmd)) return out;
    ++out.rows_done;   // row 27 bool
    if (!bool_var(objects.r28, 5, concmd)) return out;
    ++out.rows_done;   // row 28 bool
    if (!bool_var(objects.r29, 6, concmd)) return out;
    ++out.rows_done;   // row 29 bool
    if (!bool_var(objects.r30, 7, concmd)) return out;
    ++out.rows_done;   // row 30 bool
    if (!float_var(objects.r31, 23, concmd)) return out;
    ++out.rows_done;   // row 31 float
    if (!float_var(objects.r32, 24, concmd)) return out;
    ++out.rows_done;   // row 32 float
    if (!float_var(objects.r33, 25, concmd)) return out;
    ++out.rows_done;   // row 33 float
    if (!float_var(objects.r34, 26, concmd)) return out;
    ++out.rows_done;   // row 34 float
    if (!float_var(objects.r35, 27, concmd)) return out;
    ++out.rows_done;   // row 35 float
    if (!bool_var(objects.r36, 8, concmd)) return out;
    ++out.rows_done;   // row 36 bool
    if (!slider_item(objects.r37, kSlot349MenuStrings[0], kSlot349MenuStringLengths[0], objects.r00, kSlot349SliderLoBits[0], kSlot349SliderHiBits[0], concmd, menu)) return out;
    ++out.rows_done;   // row 37 slider
    if (!slider_item(objects.r38, kSlot349MenuStrings[0], kSlot349MenuStringLengths[0], objects.r01, kSlot349SliderLoBits[1], kSlot349SliderHiBits[1], concmd, menu)) return out;
    ++out.rows_done;   // row 38 slider
    if (!slider_item(objects.r39, kSlot349MenuStrings[0], kSlot349MenuStringLengths[0], objects.r35, kSlot349SliderLoBits[2], kSlot349SliderHiBits[2], concmd, menu)) return out;
    ++out.rows_done;   // row 39 slider
    if (!family_named_item_26db0(objects.r40, kSlot349MenuStrings[1], kSlot349MenuStringLengths[1], objects.r24.base, target_of(objects.r24), kSlot349ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 40 tog
    if (!family_named_item_26db0(objects.r41, kSlot349MenuStrings[1], kSlot349MenuStringLengths[1], objects.r25.base, target_of(objects.r25), kSlot349ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 41 tog
    if (!name_holder(objects.r42, kSlot349MenuStrings[2], kSlot349MenuStringLengths[2], kSlot349HolderVptr, kSlot349HolderDtorRva, concmd)) return out;
    ++out.rows_done;   // row 42 holder
    if (!name_holder(objects.r43, kSlot349MenuStrings[3], kSlot349MenuStringLengths[3], kSlot349HolderVptrB, kSlot349HolderDtorRvaB, concmd)) return out;
    ++out.rows_done;   // row 43 holder
    if (!name_holder(objects.r44, kSlot349MenuStrings[4], kSlot349MenuStringLengths[4], kSlot349HolderVptrB, kSlot349HolderDtorRvaB, concmd)) return out;
    ++out.rows_done;   // row 44 holder
    if (!name_holder(objects.r45, kSlot349MenuStrings[5], kSlot349MenuStringLengths[5], kSlot349HolderVptrB, kSlot349HolderDtorRvaB, concmd)) return out;
    ++out.rows_done;   // row 45 holder
    if (!name_holder(objects.r46, kSlot349MenuStrings[6], kSlot349MenuStringLengths[6], kSlot349HolderVptrB, kSlot349HolderDtorRvaB, concmd)) return out;
    ++out.rows_done;   // row 46 holder
    if (!name_holder(objects.r47, kSlot349MenuStrings[7], kSlot349MenuStringLengths[7], kSlot349HolderVptrB, kSlot349HolderDtorRvaB, concmd)) return out;
    ++out.rows_done;   // row 47 holder
    if (!name_holder(objects.r48, kSlot349MenuStrings[8], kSlot349MenuStringLengths[8], kSlot349HolderVptrB, kSlot349HolderDtorRvaB, concmd)) return out;
    ++out.rows_done;   // row 48 holder
    out.ok = true;
    return out;
}

}  // namespace sotc
