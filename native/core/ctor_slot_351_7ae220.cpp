#include "ctor_slot_351_7ae220.hpp"

#include <cstring>

namespace sotc {

namespace {

// 0x27120 slider over a float variable: 0x27120 (text = the variable's name view), 0x268b0 (tail), f590(label), __cxa_atexit(0x272d0).
bool slider(DebugMenuItem& item, const ConCmdRangedVar& var, MenuSiteResult& iout, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    float lo = 0.0f;
    float hi = 0.0f;
    const std::uint32_t lo_bits = kSlot351SliderLoBits;
    const std::uint32_t hi_bits = kSlot351SliderHiBits;
    std::memcpy(&lo, &lo_bits, sizeof lo);
    std::memcpy(&hi, &hi_bits, sizeof hi);
    const SharedStr label = l2::measured_view(kSlot351Parent, kSlot351ParentLength);
    iout.ctor_ok = init_debug_menu_item_27120(item, label, var.base, kSlot351ItemFlags, lo, hi);
    if (!iout.ctor_ok) return false;
    iout.registration = register_debug_menu_item_268b0(item, menu, concmd.atexit_log);
    if (!iout.registration.ok) return false;
    iout.views_inert = release_inert_view_f590(label);
    if (!iout.views_inert) return false;
    iout.atexit_recorded = concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
    iout.ok = iout.ctor_ok && iout.registration.ok && iout.views_inert && iout.atexit_recorded;
    return iout.ok;
}

}  // namespace

Slot351Result init_slot351_7ae220(Slot351Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot351Result out{};
    ConCmdBoolVar* const bvars[kSlot351BoolCount] = {&objects.bvar0, &objects.bvar1};
    ConCmdRangedVar* const fvars[kSlot351FloatCount] = {&objects.fvar0, &objects.fvar1, &objects.fvar2, &objects.fvar3,
                                                         &objects.fvar4, &objects.fvar5, &objects.fvar6};
    for (std::size_t n = 0; n < kSlot351BoolCount; ++n) {
        out.bvar[n] = lane_m6::init_bool_var_literal(*bvars[n], kSlot351BoolNames[n], kSlot351BoolNameLengths[n], "", 0, kSlot351BoolFnTag, concmd,
                                                     kSlot351BoolValues[n], kSlot351BoolValues[n]);
        if (!out.bvar[n].ok) return out;
    }
    for (std::size_t n = 0; n < kSlot351FloatCount; ++n) {
        const FloatVarSpec spec{kSlot351FloatNames[n], "", true, kSlot351FloatLoBits[n], kSlot351FloatHiBits[n], kSlot351FloatValueBits[n],
                                kSlot351FloatFnTag, kSlot351VarFlags};
        out.fvar[n] = init_float_var_static(*fvars[n], spec, concmd);
        if (!out.fvar[n].ok) return out;
    }
    if (!slider(objects.sitem0, objects.fvar5, out.sitem[0], concmd, menu)) return out;
    if (!slider(objects.sitem1, objects.fvar6, out.sitem[1], concmd, menu)) return out;
    out.ok = true;
    return out;
}

}  // namespace sotc
