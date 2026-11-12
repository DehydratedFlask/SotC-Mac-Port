#include "helper_620e10_local_int_var.hpp"

#include "helper_26db0_named_item.hpp"
#include "menu_item_26db0.hpp"

namespace sotc {

namespace {

SharedStr borrowed(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0, nullptr};
}

}  // namespace

bool is_local_int_caller_twin_tag(std::uint64_t tag, std::uint64_t own_class) noexcept {
    for (const LocalIntCallerTwin& twin : kLocalIntCallerTwins) {
        if (twin.tag == tag && twin.aliased_class == own_class) return true;
    }
    return false;
}

const LocalIntVarClass* find_local_int_var_class(std::uint64_t wrapper_rva) noexcept {
    for (const LocalIntVarClass& row : kLocalIntVarClasses) {
        if (row.wrapper_rva == wrapper_rva) return &row;
    }
    return nullptr;
}

const LocalIntVarClass* find_local_int_var_class_by_final(std::uint64_t final_vptr) noexcept {
    for (const LocalIntVarClass& row : kLocalIntVarClasses) {
        if (row.final_vptr == final_vptr) return &row;
    }
    return nullptr;
}

LocalIntVarCtorResult helper_local_int_var_ctor(std::uint64_t wrapper_rva, ConCmdRangedVar& var, const SharedStr& name,
                                                std::int32_t initial, std::int32_t min_value, std::int32_t max_value,
                                                std::uint32_t flags, const RangedVarArgs& args,
                                                const SharedStr& description, ConCmdRegistry& registry) noexcept {
    LocalIntVarCtorResult out{};
    const LocalIntVarClass* cls = find_local_int_var_class(wrapper_rva);
    if (cls == nullptr) {
        out.refusal = HelperRefusal::wrong_helper;
        return out;
    }
    // `call [tag + 8]` selects the clone body: the row's own class or a proven caller twin (int bodies, own-class operand); anything else is refused.
    if (args.fn_vptr != cls->fn_vtbl && !is_local_int_caller_twin_tag(args.fn_vptr, cls->fn_vtbl)) {
        out.refusal = HelperRefusal::unadmitted_arg_functor;
        return out;
    }
    // wrapper +0x65: [this] = final vptr (pre-store; the inner constructor overwrites it through 0x20af0 / the mid vptr)
    var.base.vptr = cls->final_vptr;

    // wrapper +0x157: call inner(this, name, this + 0xd0, &min, &max, flags, &args', description). The inner constructor is 0xc580 with a private
    // class pair; the shared model takes the int class 0x15944a0 and the two class constants are re-tagged afterwards.
    RangedVarArgs lifted = args;
    lifted.fn_vptr = kRangedIntFnVtbl;
    if (!init_ranged_int_var_c580(var, name, &var.value, min_value, max_value, flags, lifted, description)) {
        out.refusal = HelperRefusal::unadmitted_arg_functor;
        return out;
    }
    var.base.vptr = cls->mid_vptr;      // inner: lea mid table ; add 0x10 ; [this] = rax
    var.fn_vptr = cls->fn_vtbl;         // inner: lea functor class ; [this+0xc0] = rcx
    out.arg_destroy_ok = true;          // wrapper: call [class + 0x18] is the bare-ret functor destroy

    // wrapper +0x16d: [this] = final vptr ; [this+0xd0] = [this+0x68] = *init
    var.base.vptr = cls->final_vptr;
    var.value = static_cast<std::uint32_t>(initial);
    var.default_value = static_cast<std::uint32_t>(initial);
    // wrapper +0x187: call 0x20d60
    out.registration = register_concmd_20d60(var.base, registry);
    out.ok = out.arg_destroy_ok && out.registration.ok;
    return out;
}

DebugMenuTarget local_int_var_menu_target(const ConCmdRangedVar& var) noexcept {
    DebugMenuTarget target{};
    target.object = &var;
    target.known = find_local_int_var_class_by_final(var.base.vptr) != nullptr;
    target.slot3 = target.known ? kLocalIntVarSlot3Result : 0;
    target.slot6 = target.known ? kLocalIntVarSlot6Result : 0;
    return target;
}

HelperRefusal helper_26db0_named_item_local_int(DebugMenuItem& item, const SharedStr& parent,
                                                const ConCmdRangedVar& target, std::uint32_t flags) noexcept {
    const DebugMenuTarget menu_target = local_int_var_menu_target(target);
    if (!menu_target.known) return HelperRefusal::unknown_target_class;
    return init_debug_menu_item_26db0(item, parent, target.base, menu_target, flags) ? HelperRefusal::none
                                                                                       : HelperRefusal::unknown_target_class;
}

HelperIntSiteResult helper_init_local_int_var_site(ConCmdRangedVar& var, const LocalIntSiteSpec& spec,
                                                   const SharedStr& description, ConCmdRegistry& registry) noexcept {
    HelperIntSiteResult out{};
    const LocalIntVarClass* cls = find_local_int_var_class(spec.wrapper_rva);
    const SharedStr name = borrowed(spec.name, spec.name_length);
    RangedVarArgs args{};                       // four zero qwords, payload 0, aux 0
    args.fn_vptr = spec.arg_fn_tag;             // the caller's functor tag (admitted by the wrapper or refused)

    const LocalIntVarCtorResult ctor = helper_local_int_var_ctor(spec.wrapper_rva, var, name, spec.value, spec.min_value,
                                                                 spec.max_value, spec.flags, args, description, registry);
    out.refusal = ctor.refusal;
    out.ctor_ok = ctor.ok;
    out.registration = ctor.registration;
    if (!ctor.ok || cls == nullptr) return out;

    bool desc_inert = true;
    if (spec.release_description) desc_inert = release_inert_view_f590(description);   // 7ec160 order: f590(description) first
    out.functor_destroy_ok = ctor.arg_destroy_ok;                                       // call [argfn + 0x18]
    const bool name_inert = release_inert_view_f590(name);                              // f590(name)
    out.views_inert = desc_inert && name_inert;
    out.atexit_recorded = registry.atexit_log.record(cls->dtor_rva, &var);              // __cxa_atexit(dtor, obj)
    out.ok = out.ctor_ok && out.functor_destroy_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

}  // namespace sotc
