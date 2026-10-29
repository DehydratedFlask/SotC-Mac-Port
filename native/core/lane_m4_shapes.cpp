#include "lane_m4_shapes.hpp"

#include <cstring>

namespace sotc::lane_m4 {

SharedStr borrowed_view_of(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0, nullptr};
}

BoolVarResult init_bool_var_late_release(ConCmdBoolVar& var, const SharedStr& name, const SharedStr& description,
                                         std::uint64_t fn_tag, ConCmdRegistry& registry, std::uint8_t value,
                                         std::uint8_t default_value) noexcept {
    BoolVarResult out{};

    var.base.vptr = kBoolVarFinalVptr;                    // [obj] = 0x1594208 + 0x10 (overwritten by the ctor)
    init_concmd_base_20af0(var.base, name, description, 1);   // call 0x20af0(obj, name, desc, 1)

    // No mid vptr in this shape: the stores follow the constructor directly.
    var.value_ref = &var.value;                           // [obj+0x60] = obj+0xc0
    std::memset(var.zero69, 0, sizeof var.zero69);        // dword [obj+0x69] = 0
    std::memset(var.zero70, 0, sizeof var.zero70);        // ymm [obj+0x70] = 0
    var.fn_payload = 0;                                   // qword [obj+0xa0] = 0
    var.fn_vptr = fn_tag;                                 // qword [obj+0xb0] = functor tag
    var.fn_aux = 0;                                       // qword [obj+0xb8] = 0
    var.base.vptr = kBoolVarFinalVptr;                    // [obj] = 0x1594218
    var.value = value;                                    // byte [obj+0xc0]
    var.default_value = default_value;                    // byte [obj+0x68]

    out.registration = register_concmd_20d60(var.base, registry);   // call 0x20d60

    // f590(desc) THEN f590(name), both after the registration; the stack views are borrowed => inert.
    const bool desc_inert = release_inert_view_f590(description);
    const bool name_inert = release_inert_view_f590(name);
    out.views_inert = desc_inert && name_inert;

    out.atexit_recorded = registry.atexit_log.record(kBoolVarDtorRva, &var);   // __cxa_atexit(0x20b0, obj, dso)
    out.ok = out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

IntVarSiteResult init_int_var_site(ConCmdRangedVar& var, const SharedStr& name, const SharedStr& description,
                                   std::uint64_t arg_fn_tag, ConCmdRegistry& registry, std::uint32_t value,
                                   std::uint32_t default_value) noexcept {
    IntVarSiteResult out{};
    if (!unbounded_int_fn_table_lifted(arg_fn_tag)) return out;   // refuse before any write

    RangedVarArgs args{};                                 // four zero qwords, payload 0, aux 0
    args.fn_vptr = arg_fn_tag;

    var.base.vptr = kUnboundedIntFinalVptr;               // pre-stored 0x1594bf8 + 0x10 (the ctor overwrites it)
    out.worker_ok = init_unbounded_int_var_worker_3a980(var, name, kIntVarCtorFlags, args, description);
    if (!out.worker_ok) return out;
    out.description_inert = release_inert_view_f590(description);   // call 0xf590 on the description view
    out.arg_functor_destroyed = unbounded_int_fn_table_lifted(arg_fn_tag);  // call [tag+0x18]: bare ret 0xc770
    var.base.vptr = kUnboundedIntFinalVptr;               // [obj] = 0x1594c08
    var.value = value;                                    // dword [obj+0xd0]
    var.default_value = default_value;                    // dword [obj+0x68]
    out.registration = register_concmd_20d60(var.base, registry);    // call 0x20d60
    out.name_inert = release_inert_view_f590(name);                  // call 0xf590 on the name view
    out.atexit_recorded = registry.atexit_log.record(kIntVarDtorRva, &var);   // __cxa_atexit(0x18a60, obj, dso)
    out.ok = out.worker_ok && out.description_inert && out.arg_functor_destroyed && out.registration.ok &&
             out.name_inert && out.atexit_recorded;
    return out;
}

}  // namespace sotc::lane_m4
