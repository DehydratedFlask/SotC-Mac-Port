#include "ctor_slot_395_65af60.hpp"

namespace sotc {

MenuSiteResult init_toggle_site_26db0_float(DebugMenuItem& item, const ConCmdRangedVar& target, const char* parent,
                                            std::size_t parent_length, ConCmdRegistry& registry,
                                            DebugMenuRegistry& menu) noexcept {
    MenuSiteResult out{};
    const SharedStr p = l2::measured_view(parent, parent_length);
    out.ctor_ok = helper_26db0_named_item_float(item, p, target, kSlot395ItemFlags) == HelperRefusal::none;
    if (!out.ctor_ok) return out;
    out.registration = register_debug_menu_item_268b0(item, menu, registry.atexit_log);
    out.views_inert = release_inert_view_f590(p);
    out.atexit_recorded = registry.atexit_log.record(kDebugMenuItemDtorRva, &item);
    out.ok = out.ctor_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

Slot395Result init_slot395_65af60(Slot395Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    Slot395Result out{};
    out.bvar = lane_m6::init_bool_var_literal(objects.bvar, kSlot395BoolName, kSlot395BoolNameLength, "", 0, kSlot395BoolFnTag, concmd,
                                              kSlot395BoolValue, kSlot395BoolValue);
    if (!out.bvar.ok) return out;
    ConCmdRangedVar* const fvars[kSlot395FloatCount] = {&objects.fvar0, &objects.fvar1, &objects.fvar2};
    for (std::size_t n = 0; n < kSlot395FloatCount; ++n) {
        const FloatVarSpec spec{kSlot395FloatNames[n], "", true, kSlot395FloatLoBits, kSlot395FloatHiBits[n],
                                kSlot395FloatValueBits, kSlot395FloatFnTag, kSlot395VarFlags};
        out.fvar[n] = init_float_var_static(*fvars[n], spec, concmd);
        if (!out.fvar[n].ok) return out;
    }
    // The four toggles: the bool target first (the bool-class 0x26db0 site), then the three float targets.
    out.bitem = init_toggle_site_26db0(objects.bitem, objects.bvar, kSlot395Parent, concmd, menu);
    if (!out.bitem.ok) return out;
    for (std::size_t n = 0; n < kSlot395FloatCount; ++n) {
        DebugMenuItem* const items[kSlot395FloatCount] = {&objects.fitem0, &objects.fitem1, &objects.fitem2};
        out.fitem[n] = init_toggle_site_26db0_float(*items[n], *fvars[n], kSlot395Parent, kSlot395ParentLength, concmd, menu);
        if (!out.fitem[n].ok) return out;
    }
    out.ok = true;
    return out;
}

}  // namespace sotc
