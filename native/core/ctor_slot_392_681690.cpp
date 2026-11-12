#include "ctor_slot_392_681690.hpp"

#include <cstring>

namespace sotc {

Slot392Result init_slot392_681690(Slot392Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot392Result out{};
    // 0x6816a8..0x681b19: four bounded float variables (0xc300, tag 0x15cc618), each ending in __cxa_atexit(0x20f0, var)
    for (std::size_t n = 0; n < kSlot392Count; ++n) {
        const FloatVarSpec spec{kSlot392Names[n], "", true, kSlot392VarLoBits, kSlot392VarHiBits, kSlot392ValueBits[n],
                                kSlot392FloatFnTag, kSlot392VarFlags};
        out.var[n] = init_float_var_static(objects.var[n], spec, concmd);
        if (!out.var[n].ok) return out;
    }
    float lo = 0.0f;
    float hi = 0.0f;
    const std::uint32_t lo_bits = kSlot392SliderLoBits;
    const std::uint32_t hi_bits = kSlot392SliderHiBits;
    std::memcpy(&lo, &lo_bits, sizeof lo);
    std::memcpy(&hi, &hi_bits, sizeof hi);
    // 0x681b1e..0x681cae: four 0x27120 sliders (text = the variable's name), 0x268b0 (tail), f590(label), atexit 0x272d0
    for (std::size_t n = 0; n < kSlot392Count; ++n) {
        const SharedStr label = l2::measured_view(kSlot392Label, kSlot392LabelLength);
        MenuSiteResult& item = out.item[n];
        item.ctor_ok = init_debug_menu_item_27120(objects.item[n], label, objects.var[n].base, kSlot392ItemFlags, lo, hi);
        if (!item.ctor_ok) return out;
        item.registration = register_debug_menu_item_268b0(objects.item[n], menu, concmd.atexit_log);
        if (!item.registration.ok) return out;
        item.views_inert = release_inert_view_f590(label);   // the single release; the text is the variable's own view
        if (!item.views_inert) return out;
        item.atexit_recorded = concmd.atexit_log.record(kDebugMenuItemDtorRva, &objects.item[n]);
        item.ok = item.ctor_ok && item.registration.ok && item.views_inert && item.atexit_recorded;
        if (!item.ok) return out;
    }
    out.ok = true;
    return out;
}

}  // namespace sotc
