#include "lane_m6_shapes.hpp"

namespace sotc::lane_m6 {

bool is_lane_m6_float_site_tag(std::uint64_t tag) noexcept {
    return tag == kFloatSiteTag_379 || tag == kFloatSiteTag_518;
}

bool is_lane_m6_int_site_tag(std::uint64_t tag) noexcept { return tag == kIntSiteTag_569; }

BoolVarResult init_bool_var_literal(ConCmdBoolVar& var, const char* name, std::size_t name_length,
                                    const char* description, std::size_t description_length, std::uint64_t fn_tag,
                                    ConCmdRegistry& registry, std::uint8_t value,
                                    std::uint8_t default_value) noexcept {
    const SharedStr n = l2::measured_view(name, name_length);
    const SharedStr d = l2::measured_view(description, description_length);
    return init_bool_var_static(var, n, d, fn_tag, registry, value, default_value);
}

FloatVarResult init_float_var_site(ConCmdRangedVar& var, const FloatVarSpec& spec,
                                   ConCmdRegistry& registry) noexcept {
    if (!is_lane_m6_float_site_tag(spec.arg_fn_vtbl)) return FloatVarResult{};   // refused, nothing written
    FloatVarSpec equivalent = spec;
    equivalent.arg_fn_vtbl = kRangedFloatFnVtbl;   // same four slot bodies (checker-pinned); never stored
    return init_float_var_static(var, equivalent, registry);
}

DebugMenuTarget ranged_float_menu_target(const ConCmdRangedVar& var) noexcept {
    DebugMenuTarget target{};
    target.object = &var;
    target.known = var.base.vptr == kRangedFloatFinalVptr;
    target.slot3 = target.known ? kRangedFloatSlot3Result : 0;
    target.slot6 = target.known ? kRangedFloatSlot6Result : 0;
    return target;
}

IntVarResult init_int_var_site(ConCmdRangedVar& var, const IntVarSiteSpec& spec, ConCmdRegistry& registry) noexcept {
    IntVarResult out{};
    if (!is_lane_m6_int_site_tag(spec.arg_fn_vtbl)) return out;   // refused, nothing written

    const SharedStr name = l2::measured_view(spec.name, spec.name_length);
    const SharedStr description = l2::measured_view(spec.description, spec.description_length);
    RangedVarArgs args{};                              // four zero qwords, payload 0, aux 0
    args.fn_vptr = kRangedIntFnVtbl;                   // equivalent accepted tag (0x1594c70 has the same bodies)

    var.base.vptr = kRangedIntFinalVptr;               // the site pre-stores 0x1594bf8 + 0x10; the ctor overwrites it
    out.ctor_ok = init_ranged_int_var_c580(var, name, &var.value, spec.min_value, spec.max_value, spec.flags, args,
                                           description);
    if (!out.ctor_ok) return out;

    const bool desc_inert = release_inert_view_f590(description);   // f590(description)
    out.functor_destroy_ok = is_lane_m6_int_site_tag(spec.arg_fn_vtbl);   // call [argfn + 0x18] = bare ret 0xc770
    var.base.vptr = kRangedIntFinalVptr;                            // [obj] = 0x1594bf8 + 0x10
    var.value = static_cast<std::uint32_t>(spec.value);             // dword [obj+0xd0]
    var.default_value = static_cast<std::uint32_t>(spec.default_value);   // dword [obj+0x68]
    out.registration = register_concmd_20d60(var.base, registry);
    const bool name_inert = release_inert_view_f590(name);
    out.views_inert = desc_inert && name_inert;
    out.atexit_recorded = registry.atexit_log.record(kRangedIntDtorRva, &var);
    out.ok = out.ctor_ok && out.functor_destroy_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

MenuSiteResult init_toggle_site_target(DebugMenuItem& item, const DebugMenuTarget& target, const char* label,
                                       std::size_t label_length, const char* text, std::size_t text_length,
                                       ConCmdRegistry& registry, DebugMenuRegistry& menu) noexcept {
    MenuSiteResult out{};
    const SharedStr l = l2::measured_view(label, label_length);
    const SharedStr t = l2::measured_view(text, text_length);
    out.ctor_ok = init_debug_menu_item_26ea0(item, l, t, target, kL4MenuItemFlags);
    if (!out.ctor_ok) return out;
    out.registration = register_debug_menu_item_268b0(item, menu, registry.atexit_log);
    out.views_inert = release_inert_view_f590(t) && release_inert_view_f590(l);   // text first, then label
    out.atexit_recorded = registry.atexit_log.record(kDebugMenuItemDtorRva, &item);
    out.ok = out.ctor_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

}  // namespace sotc::lane_m6
