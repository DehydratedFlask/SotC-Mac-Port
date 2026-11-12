#include "helper_6aa00_unbounded_float.hpp"

#include <cstring>

namespace sotc {

namespace {

// 0x1594318 is the class every float family copy/clone slot stores and is in the delegates' lifted set.
RangedVarArgs normalised(const RangedVarArgs& args) noexcept {
    RangedVarArgs lifted = args;
    lifted.fn_vptr = kRangedFloatFnVtbl;
    return lifted;
}

SharedStr borrowed(const char* text) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(std::strlen(text)), 0,
                     nullptr};
}

}  // namespace

HelperCtorResult helper_6aa00_unbounded_float(ConCmdRangedVar& var, const SharedStr& name, void* value_ref,
                                              std::uint32_t flags, const RangedVarArgs& args,
                                              const SharedStr& description) noexcept {
    HelperCtorResult out{};
    if (!is_float_functor_tag(args.fn_vptr)) {
        out.refusal = HelperRefusal::unadmitted_arg_functor;
        return out;
    }
    if (!init_unbounded_float_var_6aa00(var, name, value_ref, flags, normalised(args), description)) {
        out.refusal = HelperRefusal::unadmitted_arg_functor;
        return out;
    }
    out.arg_destroy_ok = true;   // [tag + 0x18] == 0xc4f0 (bare ret)
    out.ok = true;
    return out;
}

HelperCtorResult helper_c300_bounded_float(ConCmdRangedVar& var, const SharedStr& name, void* value_ref,
                                           float min_value, float max_value, std::uint32_t flags,
                                           const RangedVarArgs& args, const SharedStr& description) noexcept {
    HelperCtorResult out{};
    if (!is_float_functor_tag(args.fn_vptr)) {
        out.refusal = HelperRefusal::unadmitted_arg_functor;
        return out;
    }
    if (!init_bounded_float_var_c300(var, name, value_ref, min_value, max_value, flags, normalised(args),
                                     description)) {
        out.refusal = HelperRefusal::unadmitted_arg_functor;
        return out;
    }
    out.arg_destroy_ok = true;
    out.ok = true;
    return out;
}

FloatVarResult init_float_var_static_family(ConCmdRangedVar& var, const FloatVarSpec& spec,
                                            ConCmdRegistry& registry) noexcept {
    FloatVarResult out{};
    const SharedStr name = borrowed(spec.name);
    const SharedStr description = borrowed(spec.description);
    RangedVarArgs args{};                       // four zero qwords, payload 0, aux 0
    args.fn_vptr = spec.arg_fn_vtbl;            // caller functor {0, tag, 0}

    var.base.vptr = kRangedFloatFinalVptr;      // the site pre-stores 0x15942a0 + 0x10; the ctor overwrites it
    HelperCtorResult ctor;
    if (spec.bounded) {
        float lo = 0.0f;
        float hi = 0.0f;
        std::memcpy(&lo, &spec.lo_bits, 4);
        std::memcpy(&hi, &spec.hi_bits, 4);
        ctor = helper_c300_bounded_float(var, name, &var.value, lo, hi, spec.flags, args, description);
    } else {
        ctor = helper_6aa00_unbounded_float(var, name, &var.value, spec.flags, args, description);
    }
    out.ctor_ok = ctor.ok;
    if (!out.ctor_ok) return out;

    const bool desc_inert = release_inert_view_f590(description);   // borrowed view: exact no-op
    out.functor_destroy_ok = ctor.arg_destroy_ok;                    // call [argfn + 0x18]: bare ret
    var.base.vptr = kRangedFloatFinalVptr;                           // [this] = 0x15942a0 + 0x10
    var.value = spec.default_bits;
    var.default_value = spec.default_bits;
    out.registration = register_concmd_20d60(var.base, registry);
    const bool name_inert = release_inert_view_f590(name);
    out.views_inert = desc_inert && name_inert;
    out.atexit_recorded = registry.atexit_log.record(kRangedFloatDtorRva, &var);
    out.ok = out.ctor_ok && out.functor_destroy_ok && out.registration.ok && out.views_inert &&
             out.atexit_recorded;
    return out;
}

}  // namespace sotc
