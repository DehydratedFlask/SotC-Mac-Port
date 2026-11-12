// CPU-01-HELPER-CALLEE-MODELS-04: see helper_274730_bool_var.hpp.
#include "helper_274730_bool_var.hpp"

#include <cstring>

namespace sotc {
namespace {

const char kEmptyDescription[] = "";     // stands for the NUL byte at 0xfc521a

SharedStr empty_description_view() noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(kEmptyDescription), 0, 0, nullptr};
}

SharedStr borrowed(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0, nullptr};
}

}  // namespace

BoolVar274730Result helper_274730_bool_var(ConCmdBoolVar& var, const SharedStr& name, const std::uint8_t* initial,
                                           const RangedVarArgs& args, ConCmdRegistry& registry) noexcept {
    BoolVar274730Result out{};
    // The callee runs `call [args.fn_vptr + 8]` (clone) first: only a bool-family table has a lifted slot 1. Refuse before any store.
    if (!is_bool_functor_tag(args.fn_vptr)) {
        out.refusal = HelperRefusal::unadmitted_arg_functor;
        return out;
    }
    if (initial == nullptr) {                  // the original reads *initial unconditionally (it would fault)
        out.refusal = HelperRefusal::wrong_helper;
        return out;
    }
    const std::uint8_t initial_value = *initial;      // read once, before the object is touched (the original reads it twice)

    // 0x27477c: [this] = 0x1594208 + 0x10 (r15).
    var.base.vptr = kBoolVarCtorPreVptr;
    // 0x274878: call 0x20af0(this, name, desc{0xfc521a, 0}, 1).
    const SharedStr description = empty_description_view();
    init_concmd_base_20af0(var.base, name, description, kBoolVarCtorFlags);

    // 0x27487d..0x2748b6: mid vptr, value_ref, dword [+0x69] = 0, staging class, payload 0, aux 0.
    var.base.vptr = kBoolVarMidVptr;
    var.value_ref = &var.value;
    std::memset(var.zero69, 0, sizeof var.zero69);
    var.fn_vptr = kBoolVarCtorStagingClass;
    var.fn_payload = 0;
    var.fn_aux = 0;

    // 0x2748c1..0x2748e3: ymm [+0x70] <- the four argument qwords ; q[3] != 0 makes the first qword self-referential.
    std::memcpy(var.zero70, args.q, sizeof var.zero70);
    if (args.q[3] != 0) {
        const std::uint64_t self = reinterpret_cast<std::uint64_t>(var.zero70);
        std::memcpy(var.zero70, &self, sizeof self);
    }

    // 0x2748e6..0x27495e: the functor install through the family slots, collapsed to its net state. The family copy / clone bodies store the
    // class constant 0x1594280 into dst+0x20, so the staging class written above is replaced, not kept.
    var.fn_payload = args.fn_payload;
    var.fn_vptr = kBoolVarCtorFinalFnClass;
    var.fn_aux = args.fn_aux;

    // 0x274968: call 0xf590 on the stack description view.
    out.desc_inert = release_inert_view_f590(description);
    // 0x27497b: call [staging class + 0x18] = 0xc2f0, a bare ret.
    out.arg_destroy_ok = true;

    // 0x27497e..0x274995: [this] = 0x1594218 ; byte [+0xc0] = byte [+0x68] = *initial.
    var.base.vptr = kBoolVarFinalVptr;
    var.value = initial_value;
    var.default_value = initial_value;
    // 0x274998: call 0x20d60.
    out.registration = register_concmd_20d60(var.base, registry);

    out.ok = out.desc_inert && out.arg_destroy_ok && out.registration.ok;
    return out;
}

BoolVar274730SiteResult helper_init_bool_var_274730_site(ConCmdBoolVar& var, const BoolVar274730SiteSpec& spec,
                                                         ConCmdRegistry& registry) noexcept {
    BoolVar274730SiteResult out{};
    const SharedStr name = borrowed(spec.name, spec.name_length);
    RangedVarArgs args{};
    std::memcpy(args.q, spec.q, sizeof args.q);
    args.fn_payload = spec.fn_payload;
    args.fn_vptr = spec.arg_fn_tag;
    args.fn_aux = spec.fn_aux;

    const BoolVar274730Result ctor = helper_274730_bool_var(var, name, &spec.initial, args, registry);
    out.refusal = ctor.refusal;
    out.ctor_ok = ctor.ok;
    out.registration = ctor.registration;
    if (!ctor.ok) return out;

    out.functor_destroy_ok = is_bool_functor_tag(spec.arg_fn_tag);           // 0x2777b1: call [argfn + 0x18] = 0xc2f0 (bare ret)
    const bool name_inert = release_inert_view_f590(name);                   // 0x2777b7: f590(name)
    out.views_inert = ctor.desc_inert && name_inert;
    out.atexit_recorded = registry.atexit_log.record(kBoolVarDtorRva, &var); // 0x2777cd: __cxa_atexit(0x20b0, obj, &dso)
    out.ok = out.ctor_ok && out.functor_destroy_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

}  // namespace sotc
