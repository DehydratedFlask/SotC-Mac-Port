#include "ctor_slot_396_650f20.hpp"

#include <cstring>

namespace sotc {

namespace {

// One float variable followed by its 0x27120 slider: 0xc300 block (tag 0x15cc568), then 0x27120 (text = the variable's name),
// 0x268b0 (tail), f590(label), __cxa_atexit(0x272d0).
bool float_pair(std::size_t n, ConCmdRangedVar& var, DebugMenuItem& item, FloatVarResult& vout, MenuSiteResult& iout,
                ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    const FloatVarSpec spec{kSlot396FloatNames[n], "", true, kSlot396FloatLoBits, kSlot396FloatHiBits,
                            kSlot396FloatValueBits[n], kSlot396FloatFnTag, kSlot396VarFlags};
    vout = init_float_var_static(var, spec, concmd);
    if (!vout.ok) return false;
    float lo = 0.0f;
    float hi = 0.0f;
    const std::uint32_t lo_bits = kSlot396SliderLoBits;
    const std::uint32_t hi_bits = kSlot396SliderHiBits;
    std::memcpy(&lo, &lo_bits, sizeof lo);
    std::memcpy(&hi, &hi_bits, sizeof hi);
    const SharedStr label = l2::measured_view(kSlot396Parent, kSlot396ParentLength);
    iout.ctor_ok = init_debug_menu_item_27120(item, label, var.base, kSlot396ItemFlags, lo, hi);
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

Slot396Result init_slot396_650f20(Slot396Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot396Result out{};
    ConCmdBoolVar* const bvars[kSlot396BoolCount] = {&objects.bvar0, &objects.bvar1, &objects.bvar2,
                                                      &objects.bvar3, &objects.bvar4, &objects.bvar5};
    DebugMenuItem* const bitems[kSlot396BoolCount] = {&objects.bitem0, &objects.bitem1, &objects.bitem2,
                                                       &objects.bitem3, &objects.bitem4, &objects.bitem5};
    // The original interleaves the sites as: b0 b1 b2 b3 b4 f0 f1 b5 (each bool followed by its toggle).
    const auto bool_pair = [&](std::size_t n) noexcept {
        out.bvar[n] = lane_m6::init_bool_var_literal(*bvars[n], kSlot396BoolNames[n], kSlot396BoolNameLengths[n], "", 0,
                                                     kSlot396BoolFnTag, concmd, kSlot396BoolValues[n], kSlot396BoolValues[n]);
        if (!out.bvar[n].ok) return false;
        if (n == 3) {   // 0x651413..0x651482: 0x26ea0 toggle (label `CP10-Anim`, text `Enable blend root fitting`)
            out.bitem[n] = init_toggle_site_26ea0(*bitems[n], *bvars[n], kSlot396Parent, kSlot396ToggleText, concmd, menu);
        } else {                       // 0x26db0 toggle (parent `CP10-Anim`)
            out.bitem[n] = init_toggle_site_26db0(*bitems[n], *bvars[n], kSlot396Parent, concmd, menu);
        }
        return out.bitem[n].ok;
    };
    for (std::size_t n = 0; n < 5; ++n) {
        if (!bool_pair(n)) return out;
    }
    if (!float_pair(0, objects.fvar0, objects.fitem0, out.fvar[0], out.fitem[0], concmd, menu)) return out;
    if (!float_pair(1, objects.fvar1, objects.fitem1, out.fvar[1], out.fitem[1], concmd, menu)) return out;
    if (!bool_pair(5)) return out;
    out.ok = true;
    return out;
}

}  // namespace sotc
