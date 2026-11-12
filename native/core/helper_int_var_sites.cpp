#include "helper_int_var_sites.hpp"

namespace sotc {

namespace {

SharedStr borrowed(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0, nullptr};
}

}  // namespace

HelperIntSiteResult helper_init_ranged_int_var_site(ConCmdRangedVar& var, const HelperIntSiteSpec& spec,
                                                    ConCmdRegistry& registry) noexcept {
    HelperIntSiteResult out{};
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = borrowed(spec.description, spec.description_length);
    RangedVarArgs args{};                       // four zero qwords, payload 0, aux 0
    args.fn_vptr = spec.arg_fn_tag;             // the caller's functor tag (admitted by the wrapper or refused)

    var.base.vptr = kRangedIntFinalVptr;        // the site pre-stores 0x1594bf8 + 0x10; the ctor overwrites it
    const HelperCtorResult ctor = helper_c580_ranged_int(var, name, &var.value, spec.min_value, spec.max_value,
                                                         spec.flags, args, description);
    out.refusal = ctor.refusal;
    out.ctor_ok = ctor.ok;
    if (!out.ctor_ok) return out;

    out.functor_destroy_ok = ctor.arg_destroy_ok;                   // call [argfn + 0x18]: bare ret 0xc770
    var.base.vptr = kRangedIntFinalVptr;                            // [obj] = 0x1594bf8 + 0x10
    var.value = static_cast<std::uint32_t>(spec.value);             // dword [obj+0xd0]
    var.default_value = static_cast<std::uint32_t>(spec.default_value);   // dword [obj+0x68]
    out.registration = register_concmd_20d60(var.base, registry);
    const bool desc_inert = release_inert_view_f590(description);   // f590(description) then f590(name)
    const bool name_inert = release_inert_view_f590(name);
    out.views_inert = desc_inert && name_inert;
    out.atexit_recorded = registry.atexit_log.record(kRangedIntDtorRva, &var);
    out.ok = out.ctor_ok && out.functor_destroy_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

HelperIntSiteResult helper_init_unbounded_int_var_site(ConCmdRangedVar& var, const HelperIntSiteSpec& spec,
                                                       ConCmdRegistry& registry) noexcept {
    HelperIntSiteResult out{};
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = borrowed(spec.description, spec.description_length);
    RangedVarArgs args{};
    args.fn_vptr = spec.arg_fn_tag;

    var.base.vptr = kUnboundedIntFinalVptr;     // the site pre-stores 0x1594bf8 + 0x10
    const HelperCtorResult ctor = helper_3a980_unbounded_int(var, name, spec.flags, args, description);
    out.refusal = ctor.refusal;
    out.ctor_ok = ctor.ok;
    if (!out.ctor_ok) return out;

    bool desc_inert = true;
    if (spec.early_description_release) desc_inert = release_inert_view_f590(description);   // f590(description) before the destroy
    out.functor_destroy_ok = ctor.arg_destroy_ok;                    // call [argfn + 0x18]: bare ret 0xc770
    var.base.vptr = kUnboundedIntFinalVptr;                          // [obj] = 0x1594c08
    var.value = static_cast<std::uint32_t>(spec.value);
    var.default_value = static_cast<std::uint32_t>(spec.default_value);
    out.registration = register_concmd_20d60(var.base, registry);
    if (!spec.early_description_release) desc_inert = release_inert_view_f590(description);
    const bool name_inert = release_inert_view_f590(name);
    out.views_inert = desc_inert && name_inert;
    out.atexit_recorded = registry.atexit_log.record(kRangedIntDtorRva, &var);
    out.ok = out.ctor_ok && out.functor_destroy_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

HelperFloatSiteResult helper_init_unbounded_float_var_site(ConCmdRangedVar& var, const HelperFloatSiteSpec& spec,
                                                           ConCmdRegistry& registry) noexcept {
    HelperFloatSiteResult out{};
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = borrowed(spec.description, spec.description_length);
    RangedVarArgs args{};                       // four zero qwords, payload 0, aux 0
    args.fn_vptr = spec.arg_fn_tag;

    var.base.vptr = kRangedFloatFinalVptr;      // the site pre-stores 0x15942a0 + 0x10; the ctor overwrites it
    const HelperCtorResult ctor = helper_6aa00_unbounded_float(var, name, &var.value, spec.flags, args, description);
    out.refusal = ctor.refusal;
    out.ctor_ok = ctor.ok;
    if (!out.ctor_ok) return out;

    bool desc_inert = true;
    if (spec.early_description_release) desc_inert = release_inert_view_f590(description);
    out.functor_destroy_ok = ctor.arg_destroy_ok;                   // call [argfn + 0x18]: bare ret 0xc4f0
    var.base.vptr = kRangedFloatFinalVptr;                          // [obj] = 0x15942a0 + 0x10
    var.value = spec.value_bits;                                    // dword [obj+0xd0]
    var.default_value = spec.value_bits;                            // dword [obj+0x68]
    out.registration = register_concmd_20d60(var.base, registry);
    if (!spec.early_description_release) desc_inert = release_inert_view_f590(description);
    const bool name_inert = release_inert_view_f590(name);
    out.views_inert = desc_inert && name_inert;
    out.atexit_recorded = registry.atexit_log.record(kRangedFloatDtorRva, &var);
    out.ok = out.ctor_ok && out.functor_destroy_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

}  // namespace sotc
