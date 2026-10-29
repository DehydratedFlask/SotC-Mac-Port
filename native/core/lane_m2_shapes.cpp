#include "lane_m2_shapes.hpp"

#include <cstring>

namespace sotc::lane_m2 {

namespace {

SharedStr borrowed(const char* text) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(std::strlen(text)), 0,
                     nullptr};
}

}  // namespace

BoolVarResult init_bool_var_flagged(ConCmdBoolVar& var, const char* name, std::uint64_t fn_tag, std::uint32_t flags,
                                    ConCmdRegistry& registry, std::uint8_t value,
                                    std::uint8_t default_value) noexcept {
    BoolVarResult out{};
    const SharedStr n = borrowed(name);
    const SharedStr d = borrowed("");

    var.base.vptr = kBoolVarFinalVptr;                    // [obj] = 0x1594208 + 0x10 (overwritten by the base ctor)
    init_concmd_base_20af0(var.base, n, d, flags);        // call 0x20af0, ecx = flags

    var.base.vptr = kBoolVarMidVptr;                      // [obj] = 0x15940a0 + 0x10
    var.value_ref = &var.value;                           // [obj+0x60] = obj+0xc0
    std::memset(var.zero69, 0, sizeof var.zero69);        // dword [obj+0x69] = 0
    std::memset(var.zero70, 0, sizeof var.zero70);        // ymm [obj+0x70] = 0
    var.fn_payload = 0;                                   // qword [obj+0xa0] = 0
    var.fn_vptr = fn_tag;                                 // qword [obj+0xb0] = functor vtable
    var.fn_aux = 0;                                       // qword [obj+0xb8] = 0

    const bool desc_inert = release_inert_view_f590(d);   // call 0xf590 (description)

    var.base.vptr = kBoolVarFinalVptr;                    // [obj] = 0x1594208 + 0x10
    var.value = value;                                    // byte [obj+0xc0]
    var.default_value = default_value;                    // byte [obj+0x68]

    out.registration = register_concmd_20d60(var.base, registry);   // call 0x20d60
    const bool name_inert = release_inert_view_f590(n);             // call 0xf590 (name)
    out.views_inert = desc_inert && name_inert;
    out.atexit_recorded = registry.atexit_log.record(kBoolVarDtorRva, &var);   // __cxa_atexit(0x20b0, obj, dso)
    out.ok = out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

bool is_m2_float_arg_functor(std::uint64_t vtbl) noexcept {
    return vtbl == kFloatArgFnVtbl_15cb148 || is_known_float_arg_functor(vtbl);
}

FloatVarResult init_float_var_site(ConCmdRangedVar& var, const FloatVarSpec& spec,
                                   ConCmdRegistry& registry) noexcept {
    if (!is_m2_float_arg_functor(spec.arg_fn_vtbl)) return FloatVarResult{};
    FloatVarSpec shared = spec;
    // Output-neutral: see the header. 0x15ce318 has the same four lifted slot bodies as 0x15cb148.
    if (shared.arg_fn_vtbl == kFloatArgFnVtbl_15cb148) shared.arg_fn_vtbl = kFloatArgFnVtbl_347;
    return init_float_var_static(var, shared, registry);
}

bool is_m2_int_arg_functor(std::uint64_t table) noexcept {
    return table == kIntArgFnTable_15cb128 || unbounded_int_fn_table_lifted(table);
}

IntVarResult init_unbounded_int_var_site(ConCmdRangedVar& var, const IntVarSpec& spec,
                                         ConCmdRegistry& registry) noexcept {
    IntVarResult out{};
    if (!is_m2_int_arg_functor(spec.arg_fn_table)) return out;   // refused before any write
    const SharedStr name = borrowed(spec.name);
    const SharedStr description = borrowed(spec.description);
    RangedVarArgs args{};                     // four zero qwords, payload 0, aux 0
    // Output-neutral substitution (see the header): the worker only gates on the table and then installs 0x15944a0.
    args.fn_vptr = spec.arg_fn_table == kIntArgFnTable_15cb128 ? kUnboundedIntFnTableB : spec.arg_fn_table;

    var.base.vptr = kUnboundedIntFinalVptr;   // [obj] = 0x1594bf8 + 0x10 (overwritten by the base ctor)
    out.worker_ok = init_unbounded_int_var_worker_3a980(var, name, spec.flags, args, description);
    if (!out.worker_ok) return out;
    out.description_inert = release_inert_view_f590(description);          // call 0xf590
    out.functor_destroy_ok = is_m2_int_arg_functor(spec.arg_fn_table);     // call [table + 0x18]: bare ret
    var.base.vptr = kUnboundedIntFinalVptr;   // [obj] = 0x1594bf8 + 0x10
    var.value = spec.value;                   // dword [obj+0xd0]
    var.default_value = spec.default_value;   // dword [obj+0x68]
    out.registration = register_concmd_20d60(var.base, registry);          // call 0x20d60
    out.name_inert = release_inert_view_f590(name);                        // call 0xf590
    out.atexit_recorded = registry.atexit_log.record(0x18a60, &var);       // __cxa_atexit(0x18a60, obj, dso)
    out.ok = out.worker_ok && out.description_inert && out.functor_destroy_ok && out.registration.ok &&
             out.name_inert && out.atexit_recorded;
    return out;
}

}  // namespace sotc::lane_m2
