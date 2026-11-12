#include "helper_140b00_twin_site.hpp"

namespace sotc {

namespace {

SharedStr borrowed(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0, nullptr};
}

}  // namespace

HelperIntSiteResult helper_init_unbounded_int_twin_var_site(ConCmdRangedVar& var, UnboundedIntTwinVariant variant, const HelperIntTwinSiteSpec& spec,
                                                            std::uint32_t* value_cell, ConCmdRegistry& registry) noexcept {
    HelperIntSiteResult out{};
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = borrowed(spec.description, spec.description_length);
    RangedVarArgs args{};                                         // four zero qwords, payload 0, aux 0
    args.fn_vptr = unbounded_int_twin_seed_class(variant);        // every observed site passes its own seed class
    std::uint32_t* const cell = value_cell != nullptr ? value_cell : &var.value;

    if (spec.pre_store_vptr) var.base.vptr = spec.final_vptr;     // pre-store (overwritten by the base constructor); twin B has none
    const HelperCtorResult ctor = helper_unbounded_int_twin_140b00(var, variant, name, spec.flags, args, description);
    out.refusal = ctor.refusal;
    out.ctor_ok = ctor.ok;
    if (!out.ctor_ok) return out;

    var.value_ref = cell;                                         // rdx = the value cell (the constructor stores it at +0x60)
    bool desc_inert = true;
    if (spec.early_description_release) desc_inert = release_inert_view_f590(description);
    out.functor_destroy_ok = ctor.arg_destroy_ok;                 // call [seed + 0x18]: bare ret
    var.base.vptr = spec.final_vptr;                              // [obj] = final class
    if (spec.write_value) *cell = static_cast<std::uint32_t>(spec.value);
    var.default_value = spec.write_value ? static_cast<std::uint32_t>(spec.default_value) : *cell;   // dword [obj+0x68] (twin B: loaded from the external cell)
    out.registration = register_concmd_20d60(var.base, registry);
    if (!spec.early_description_release) desc_inert = release_inert_view_f590(description);
    const bool name_inert = release_inert_view_f590(name);
    out.views_inert = desc_inert && name_inert;
    out.atexit_recorded = registry.atexit_log.record(spec.dtor_rva, &var);
    out.ok = out.ctor_ok && out.functor_destroy_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

}  // namespace sotc
