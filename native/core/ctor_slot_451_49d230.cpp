#include "ctor_slot_451_49d230.hpp"

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
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot451ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}


[[maybe_unused]] bool uint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot451UIntDesc[n];   // d < 0: the empty description; otherwise a borrowed view of the menu string literal
    const HelperIntSiteSpec spec{kSlot451UIntNames[n], kSlot451UIntNameLengths[n], d < 0 ? "" : kSlot451MenuStrings[d], d < 0 ? 0 : kSlot451MenuStringLengths[d], 0, 0, kSlot451VarFlags, kSlot451UIntFnTag, kSlot451UIntValue[n], kSlot451UIntValue[n], kSlot451UIntEarly[n] != 0};
    return helper_init_unbounded_int_var_site(var, spec, concmd).ok;
}



// Console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], two f590, __cxa_atexit(0x4b0)); the handler is an identity tag, never called.
[[maybe_unused]] bool cmd_var(ConCmdCommand& command, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot451CmdDesc[n];
    const FConCmdCommandSpec spec{kSlot451CmdNames[n], kSlot451MenuStrings[d], kSlot451MenuStringLengths[d], kSlot451CmdHandler[n]};
    const FConCmdBuildResult built = init_fconcmd_object(command, spec, concmd);
    return built.ok && built.vtable_register_slot_ok && built.views_inert && built.atexit_recorded && !built.handler_invoked;
}


[[maybe_unused]] bool ufloat_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot451UFloatDesc[n];
    const char* const desc = d < 0 ? "" : kSlot451MenuStrings[d];
    const std::size_t desc_length = d < 0 ? 0 : kSlot451MenuStringLengths[d];
    const HelperFloatSiteSpec spec{kSlot451UFloatNames[n], kSlot451UFloatNameLengths[n], desc, desc_length, kSlot451VarFlags, kSlot451UFloatFnTag, kSlot451UFloatValueBits[n], kSlot451UFloatEarly[n] != 0};
    return helper_init_unbounded_float_var_site(var, spec, concmd).ok;
}




[[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot451BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
    return lane_m6::init_bool_var_literal(var, kSlot451BoolNames[n], kSlot451BoolNameLengths[n], d < 0 ? "" : kSlot451MenuStrings[d], d < 0 ? 0 : kSlot451MenuStringLengths[d],
                                          kSlot451BoolFnTag, concmd, kSlot451BoolValues[n], kSlot451BoolValues[n], kSlot451BoolEarly[n] != 0).ok;
}


// Dword-ranged variable (0x49ca60 / 0x49cc60 -> bare-ret functor destroy -> final vptr -> value/default -> 0x20d60 -> f590(description) -> f590(name) -> __cxa_atexit(0x498b20 / 0x498b60)).
bool dwr_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {
    const std::int32_t d = kSlot451DwrDesc[n];
    const HelperDwordSiteSpec spec{kSlot451DwrVariant[n] == 0 ? RangedDwordVariant::a_49ca60 : RangedDwordVariant::b_49cc60, kSlot451DwrNames[n], kSlot451DwrNameLengths[n],
                                   kSlot451MenuStrings[d], kSlot451MenuStringLengths[d], kSlot451VarFlags, kSlot451DwrFnTag[n], kSlot451DwrMin[n], kSlot451DwrMax[n], kSlot451DwrValue[n], kSlot451DwrDefault[n],
                                   kSlot451DwrFinalVptr[n], kSlot451DwrDtorRva[n]};
    return helper_init_ranged_dword_var_site(var, spec, concmd).ok;
}

// Critical-section object: 0x3b310(obj) fills the 8 byte handle at +0, then the observed field stores (offset / width / value tables) are replayed; __cxa_atexit(dtor, obj) is recorded
// (0x499e50 / 0x499e60 are identity tags and never run). Nothing else in the object is written.
template <std::size_t N>
bool crit_row(Slot451Crit<N>& c, std::size_t n, ConCmdRegistry& concmd) noexcept {
    if (kSlot451CritSize[n] != N) return false;
    if (critsec_init_at(c.b) != 0) return false;
    for (std::size_t s = kSlot451CritStoreFirst[n]; s < kSlot451CritStoreFirst[n + 1]; ++s) {
        const std::uint32_t off = kSlot451CritStoreOffset[s];
        const std::uint32_t width = kSlot451CritStoreWidth[s];
        if (off < 8 || off + width > N) return false;
        if (width <= 8) {
            const std::uint64_t value = kSlot451CritStoreValue[s];
            std::memcpy(c.b + off, &value, width);
        } else {
            std::memset(c.b + off, 0, width);   // a vector store of zero (the tables only admit zero past 8 bytes)
        }
    }
    return concmd.atexit_log.record(kSlot451CritDtorRva[n], &c);
}

// Zero pointer slot: the original stores qword 0 and records __cxa_atexit(0x565e80, &slot) (identity tag; the destructor frees the pointee, never run).
bool zero_pointer(std::uint64_t& slot, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {
    slot = 0;
    return concmd.atexit_log.record(dtor, &slot);
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot451Result init_slot451_49d230(Slot451Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot451Result out{};

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
    if (!uint_var(objects.r05, 3, concmd)) return out;
    ++out.rows_done;   // row 05 uint
    if (!ufloat_var(objects.r06, 0, concmd)) return out;
    ++out.rows_done;   // row 06 ufloat
    if (!ufloat_var(objects.r07, 1, concmd)) return out;
    ++out.rows_done;   // row 07 ufloat
    if (!uint_var(objects.r08, 4, concmd)) return out;
    ++out.rows_done;   // row 08 uint
    if (!dwr_var(objects.r09, 0, concmd)) return out;
    ++out.rows_done;   // row 09 dwr
    if (!ufloat_var(objects.r10, 2, concmd)) return out;
    ++out.rows_done;   // row 10 ufloat
    if (!bool_var(objects.r11, 2, concmd)) return out;
    ++out.rows_done;   // row 11 bool
    if (!bool_var(objects.r12, 3, concmd)) return out;
    ++out.rows_done;   // row 12 bool
    if (!dwr_var(objects.r13, 1, concmd)) return out;
    ++out.rows_done;   // row 13 dwr
    if (!bool_var(objects.r14, 4, concmd)) return out;
    ++out.rows_done;   // row 14 bool
    if (!bool_var(objects.r15, 5, concmd)) return out;
    ++out.rows_done;   // row 15 bool
    if (!bool_var(objects.r16, 6, concmd)) return out;
    ++out.rows_done;   // row 16 bool
    if (!bool_var(objects.r17, 7, concmd)) return out;
    ++out.rows_done;   // row 17 bool
    if (!ufloat_var(objects.r18, 3, concmd)) return out;
    ++out.rows_done;   // row 18 ufloat
    if (!crit_row(objects.r19, 0, concmd)) return out;
    ++out.rows_done;   // row 19 crit
    if (!crit_row(objects.r20, 1, concmd)) return out;
    ++out.rows_done;   // row 20 crit
    if (!zero_pointer(objects.r21, kSlot451ZeroPtrDtorRva, concmd)) return out;
    ++out.rows_done;   // row 21 zq
    if (!cmd_var(objects.r22, 0, concmd)) return out;
    ++out.rows_done;   // row 22 cmd
    out.ok = true;
    return out;
}


}  // namespace sotc
