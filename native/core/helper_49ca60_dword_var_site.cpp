// CPU-01-AOT-CFG-SCALE-01-BATCH17: see helper_49ca60_dword_var_site.hpp.
#include "helper_49ca60_dword_var_site.hpp"

namespace sotc {

namespace {
SharedStr borrowed(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0, nullptr};
}
}  // namespace

HelperDwordSiteResult helper_init_ranged_dword_var_site(ConCmdRangedVar& var, const HelperDwordSiteSpec& spec,
                                                        ConCmdRegistry& registry) noexcept {
    HelperDwordSiteResult out{};
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = borrowed(spec.description, spec.description_length);
    RangedVarArgs args{};                        // four zero qwords, payload 0, aux 0
    args.fn_vptr = spec.arg_fn_tag;              // the caller's functor tag (admitted by the constructor or refused, nothing written)
    const std::int32_t lo = spec.min_value;
    const std::int32_t hi = spec.max_value;

    const RangedDwordVarResult ctor = helper_ranged_dword_var_49ca60(
        var, spec.variant, name, &var.value, reinterpret_cast<const std::uint8_t*>(&lo), reinterpret_cast<const std::uint8_t*>(&hi), spec.flags, args,
        description);
    out.refusal = ctor.refusal;
    out.ctor_ok = ctor.ok;
    if (!out.ctor_ok) return out;

    // `call [class + 0x18]` is the bare-ret destroy slot (0x49cc50 / 0x49ce50, pinned): no state change.
    var.base.vptr = spec.final_vptr;                                   // movq rbx, (r14)
    var.value = static_cast<std::uint32_t>(spec.value);                // dword [obj+0xd0]
    var.default_value = static_cast<std::uint32_t>(spec.default_value);  // dword [obj+0x68]
    out.registration = register_concmd_20d60(var.base, registry);
    const bool desc_inert = release_inert_view_f590(description);       // late order: f590(description) then f590(name)
    const bool name_inert = release_inert_view_f590(name);
    out.views_inert = desc_inert && name_inert;
    out.atexit_recorded = registry.atexit_log.record(spec.dtor_rva, &var);
    out.ok = out.ctor_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

}  // namespace sotc
