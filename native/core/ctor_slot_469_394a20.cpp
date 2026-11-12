#include "ctor_slot_469_394a20.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot469ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}

// The 0x27050 slider: ctor(item, label, text, target, 0x30, lo, hi) (explicit text, any target object), 0x268b0, f590(text), f590(label), __cxa_atexit(0x272d0, item).
[[maybe_unused]] bool slider50_item(DebugMenuItem& item, const char* label, std::size_t label_length, const char* text, std::size_t text_length, const void* target,
                                    std::uint32_t lo_bits, std::uint32_t hi_bits, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    float lo = 0.0f;
    float hi = 0.0f;
    std::memcpy(&lo, &lo_bits, sizeof lo);
    std::memcpy(&hi, &hi_bits, sizeof hi);
    const SharedStr label_view = family_view(label, label_length);
    const SharedStr text_view = family_view(text, text_length);
    if (!init_debug_menu_item_27050(item, label_view, text_view, target, kSlot469ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(text_view) || !release_inert_view_f590(label_view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}




[[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const FloatVarSpec spec{kSlot469FloatNames[n], "", true, kSlot469FloatLoBits[n], kSlot469FloatHiBits[n], kSlot469FloatValueBits[n], kSlot469FloatFnTag, kSlot469VarFlags};
    return init_float_var_static(var, spec, concmd).ok;
}




[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    return lane_m6::init_bool_var_literal(var, kSlot469BoolNames[n], kSlot469BoolNameLengths[n], "", 0, kSlot469BoolFnTag, concmd, kSlot469BoolValues[n],
                                          kSlot469BoolValues[n]).ok;
}

// Sentinel container: the self-referential header stores of the original (q20, then q00 = q08, then q10); the +0x18 field and the 0x80 B area are never written. __cxa_atexit(dtor, &container) is recorded.
bool sentinel_container(Slot469Sentinel& c, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {
    const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(&c);
    c.q20 = base + 0x28;
    c.q00 = base + 0x28;
    c.q08 = base + 0x28;
    c.q10 = base + 0xa8;
    return concmd.atexit_log.record(dtor, &c);
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot469Result init_slot469_394a20(Slot469Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot469Result out{};

    if (!bool_var(objects.r00, 0, concmd)) return out;
    ++out.rows_done;   // row 00 bool
    if (!bool_var(objects.r01, 1, concmd)) return out;
    ++out.rows_done;   // row 01 bool
    if (!bool_var(objects.r02, 2, concmd)) return out;
    ++out.rows_done;   // row 02 bool
    if (!float_var(objects.r03, 0, concmd)) return out;
    ++out.rows_done;   // row 03 float
    if (!float_var(objects.r04, 1, concmd)) return out;
    ++out.rows_done;   // row 04 float
    if (!bool_var(objects.r05, 3, concmd)) return out;
    ++out.rows_done;   // row 05 bool
    if (!bool_var(objects.r06, 4, concmd)) return out;
    ++out.rows_done;   // row 06 bool
    if (!float_var(objects.r07, 2, concmd)) return out;
    ++out.rows_done;   // row 07 float
    if (!family_group_item_26c10(objects.r08, kSlot469MenuStrings[0], kSlot469MenuStringLengths[0], kSlot469MenuStrings[1], kSlot469MenuStringLengths[1], kSlot469ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 08 group
    if (!family_toggle_item_26ea0(objects.r09, kSlot469MenuStrings[1], kSlot469MenuStringLengths[1], kSlot469MenuStrings[2], kSlot469MenuStringLengths[2], target_of(objects.r00), kSlot469ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 09 toge
    if (!family_toggle_item_26ea0(objects.r10, kSlot469MenuStrings[1], kSlot469MenuStringLengths[1], kSlot469MenuStrings[3], kSlot469MenuStringLengths[3], target_of(objects.r01), kSlot469ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 10 toge
    if (!family_toggle_item_26ea0(objects.r11, kSlot469MenuStrings[1], kSlot469MenuStringLengths[1], kSlot469MenuStrings[4], kSlot469MenuStringLengths[4], target_of(objects.r02), kSlot469ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 11 toge
    if (!(objects.external_target_0 != nullptr && slider50_item(objects.r12, kSlot469MenuStrings[1], kSlot469MenuStringLengths[1], kSlot469MenuStrings[5], kSlot469MenuStringLengths[5], objects.external_target_0, kSlot469SliderLoBits[0], kSlot469SliderHiBits[0], concmd, menu))) return out;
    ++out.rows_done;   // row 12 slider50
    if (!slider50_item(objects.r13, kSlot469MenuStrings[1], kSlot469MenuStringLengths[1], kSlot469MenuStrings[6], kSlot469MenuStringLengths[6], &objects.r03, kSlot469SliderLoBits[1], kSlot469SliderHiBits[1], concmd, menu)) return out;
    ++out.rows_done;   // row 13 slider50
    if (!slider50_item(objects.r14, kSlot469MenuStrings[1], kSlot469MenuStringLengths[1], kSlot469MenuStrings[7], kSlot469MenuStringLengths[7], &objects.r04, kSlot469SliderLoBits[2], kSlot469SliderHiBits[2], concmd, menu)) return out;
    ++out.rows_done;   // row 14 slider50
    if (!family_toggle_item_26ea0(objects.r15, kSlot469MenuStrings[1], kSlot469MenuStringLengths[1], kSlot469MenuStrings[8], kSlot469MenuStringLengths[8], target_of(objects.r05), kSlot469ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 15 toge
    if (!family_toggle_item_26ea0(objects.r16, kSlot469MenuStrings[1], kSlot469MenuStringLengths[1], kSlot469MenuStrings[9], kSlot469MenuStringLengths[9], target_of(objects.r06), kSlot469ItemFlags, concmd, menu).ok()) return out;
    ++out.rows_done;   // row 16 toge
    if (!slider50_item(objects.r17, kSlot469MenuStrings[1], kSlot469MenuStringLengths[1], kSlot469MenuStrings[10], kSlot469MenuStringLengths[10], &objects.r07, kSlot469SliderLoBits[3], kSlot469SliderHiBits[3], concmd, menu)) return out;
    ++out.rows_done;   // row 17 slider50
    if (!sentinel_container(objects.r18, kSlot469SentinelDtorRva, concmd)) return out;
    ++out.rows_done;   // row 18 sent
    out.ok = true;
    return out;
}


}  // namespace sotc
