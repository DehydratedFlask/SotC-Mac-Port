#include "helper_233ec0_unbounded_int_twin.hpp"

#include "helper_3a980_unbounded_int.hpp"

namespace sotc {

namespace {

SharedStr borrowed(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0, nullptr};
}

}  // namespace

HelperCtorResult helper_233ec0_unbounded_int_twin(ConCmdRangedVar& var, const SharedStr& name, std::uint32_t flags,
                                                  const RangedVarArgs& block, const SharedStr& description) noexcept {
    // Same body as 0x3a980 outside the class pair: the install reaches the caller functor only through `call [tag + 8]`. The private class
    // 0x159b680 has the int family's four slot shapes; the shared model takes a family tag and the stored class is re-tagged afterwards.
    const bool private_class = block.fn_vptr == kUnboundedIntTwinSeedFnTable;
    RangedVarArgs lifted = block;
    if (private_class) lifted.fn_vptr = kIntFunctorClassTag;
    HelperCtorResult out = helper_3a980_unbounded_int(var, name, flags, lifted, description);
    if (!out.ok) return out;                          // nothing written
    var.base.vptr = kUnboundedIntTwinMidVptr;         // 0x233ef1: lea 0x159b590 ; add 0x10 ; [this] = rax
    if (private_class) var.fn_vptr = kUnboundedIntTwinSeedFnTable;   // 0x234050 stores class 0x159b680 into the object
    return out;
}

DebugMenuTarget unbounded_int_twin_menu_target(const ConCmdRangedVar& var) noexcept {
    DebugMenuTarget target{};
    target.object = &var;
    target.known = var.base.vptr == kUnboundedIntTwinFinalVptr;
    target.slot3 = target.known ? kUnboundedIntTwinSlot3Result : 0;
    target.slot6 = target.known ? kUnboundedIntTwinSlot6Result : 0;
    return target;
}

HelperIntSiteResult helper_init_unbounded_int_twin_site(ConCmdRangedVar& var, const HelperIntSiteSpec& spec,
                                                        ConCmdRegistry& registry) noexcept {
    HelperIntSiteResult out{};
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = borrowed(spec.description, spec.description_length);
    RangedVarArgs args{};
    args.fn_vptr = spec.arg_fn_tag;

    var.base.vptr = kUnboundedIntTwinFinalVptr;     // 0x23414c: the site pre-stores 0x159b608 + 0x10 (rbx)
    const HelperCtorResult ctor = helper_233ec0_unbounded_int_twin(var, name, spec.flags, args, description);
    out.refusal = ctor.refusal;
    out.ctor_ok = ctor.ok;
    if (!out.ctor_ok) return out;

    const bool desc_inert = release_inert_view_f590(description);   // f590(description) before the destroy
    out.functor_destroy_ok = ctor.arg_destroy_ok;                    // call [argfn + 0x18]: bare ret 0xc770
    var.base.vptr = kUnboundedIntTwinFinalVptr;                      // [obj] = 0x159b618
    var.value = static_cast<std::uint32_t>(spec.value);
    var.default_value = static_cast<std::uint32_t>(spec.default_value);
    out.registration = register_concmd_20d60(var.base, registry);
    const bool name_inert = release_inert_view_f590(name);
    out.views_inert = desc_inert && name_inert;
    out.atexit_recorded = registry.atexit_log.record(kUnboundedIntTwinDtorRva, &var);
    out.ok = out.ctor_ok && out.functor_destroy_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

}  // namespace sotc
