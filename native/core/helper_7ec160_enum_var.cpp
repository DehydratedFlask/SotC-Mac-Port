#include "helper_7ec160_enum_var.hpp"

namespace sotc {

namespace {

SharedStr borrowed(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0, nullptr};
}

}  // namespace

EnumVarCtorResult helper_7ec160_enum_var(ConCmdRangedVar& var, const SharedStr& name, std::int32_t initial,
                                         std::int32_t min_value, std::int32_t max_value, std::uint32_t flags,
                                         const RangedVarArgs& args, const SharedStr& description,
                                         ConCmdRegistry& registry) noexcept {
    EnumVarCtorResult out{};
    // `call [tag + 8]` selects the clone body: only the lifted class table 0x15ce538 is accepted, anything else would run an unread slot.
    if (args.fn_vptr != kEnumVarFnVtbl) {
        out.refusal = HelperRefusal::unadmitted_arg_functor;
        return out;
    }
    // 0x7ec1c5: [this] = 0x15ce558 + 0x10 (pre-store; 0x7ec320 overwrites it through 0x20af0 / the mid vptr)
    var.base.vptr = kEnumVarFinalVptr;

    // 0x7ec2b7: call 0x7ec320(this, name, this + 0xd0, &min, &max, flags, &args', description). 0x7ec320 is 0xc580 with the class pair
    // (0x15ce4d0, 0x15ce538); the shared model takes the int class 0x15944a0 and the two class constants are re-tagged afterwards.
    RangedVarArgs lifted = args;
    lifted.fn_vptr = kRangedIntFnVtbl;
    if (!init_ranged_int_var_c580(var, name, &var.value, min_value, max_value, flags, lifted, description)) {
        out.refusal = HelperRefusal::unadmitted_arg_functor;
        return out;
    }
    var.base.vptr = kEnumVarMidVptr;     // 0x7ec35b: lea 0x15ce4c0 ; add 0x10 ; [this] = rax
    var.fn_vptr = kEnumVarFnVtbl;        // 0x7ec366: lea 0x15ce538 ; [this+0xc0] = rcx
    out.arg_destroy_ok = true;           // 0x7ec2ca: call [0x15ce538 + 0x18] = 0x7ec150 (bare ret)

    // 0x7ec2cd..0x7ec2e4: [this] = final vptr ; [this+0xd0] = [this+0x68] = *init
    var.base.vptr = kEnumVarFinalVptr;
    var.value = static_cast<std::uint32_t>(initial);
    var.default_value = static_cast<std::uint32_t>(initial);
    // 0x7ec2e7: call 0x20d60
    out.registration = register_concmd_20d60(var.base, registry);
    out.ok = out.arg_destroy_ok && out.registration.ok;
    return out;
}

DebugMenuTarget enum_var_menu_target(const ConCmdRangedVar& var) noexcept {
    DebugMenuTarget target{};
    target.object = &var;
    target.known = var.base.vptr == kEnumVarFinalVptr;
    target.slot3 = target.known ? kEnumVarSlot3Result : 0;
    target.slot6 = target.known ? kEnumVarSlot6Result : 0;
    return target;
}

HelperIntSiteResult helper_init_enum_var_site(ConCmdRangedVar& var, const HelperIntSiteSpec& spec,
                                              ConCmdRegistry& registry) noexcept {
    HelperIntSiteResult out{};
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = borrowed(spec.description, spec.description_length);
    RangedVarArgs args{};                       // four zero qwords, payload 0, aux 0
    args.fn_vptr = spec.arg_fn_tag;             // the caller's functor tag (admitted by the wrapper or refused)

    const EnumVarCtorResult ctor = helper_7ec160_enum_var(var, name, spec.value, spec.min_value, spec.max_value, spec.flags,
                                                          args, description, registry);
    out.refusal = ctor.refusal;
    out.ctor_ok = ctor.ok;
    out.registration = ctor.registration;
    if (!ctor.ok) return out;

    const bool desc_inert = release_inert_view_f590(description);   // 0x7ec5b4: f590(description)
    out.functor_destroy_ok = ctor.arg_destroy_ok;                    // 0x7ec5c4: call [argfn + 0x18]
    const bool name_inert = release_inert_view_f590(name);          // 0x7ec5ca: f590(name)
    out.views_inert = desc_inert && name_inert;
    out.atexit_recorded = registry.atexit_log.record(kEnumVarDtorRva, &var);   // 0x7ec5e3: __cxa_atexit(0x7ea160, obj)
    out.ok = out.ctor_ok && out.functor_destroy_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

}  // namespace sotc
