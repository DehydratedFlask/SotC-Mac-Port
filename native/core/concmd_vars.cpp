#include "concmd_vars.hpp"

#include <cstring>

namespace sotc {

BoolVarResult init_bool_var_static(ConCmdBoolVar& var, const SharedStr& name,
                                   const SharedStr& description, std::uint64_t fn_vptr_tag,
                                   ConCmdRegistry& registry, std::uint8_t value,
                                   std::uint8_t default_value) noexcept {
    BoolVarResult out{};

    // 0xac9595..0xac95d3: [obj] = 0x1594218 (overwritten by the base constructor), then
    // call 0x20af0(obj, name, description, 1).
    var.base.vptr = kBoolVarFinalVptr;
    init_concmd_base_20af0(var.base, name, description, 1);

    // 0xac95f8..0xac962a, in this order.
    var.base.vptr = kBoolVarMidVptr;                      // [obj] = 0x15940b0
    var.value_ref = &var.value;                           // [obj+0x60] = &value (obj+0xc0)
    std::memset(var.zero69, 0, sizeof var.zero69);        // dword [obj+0x69] = 0
    std::memset(var.zero70, 0, sizeof var.zero70);        // ymm [obj+0x70] = 0
    var.fn_payload = 0;                                   // qword [obj+0xa0] = 0
    var.fn_vptr = fn_vptr_tag;                            // qword [obj+0xb0] = functor vtable
    var.fn_aux = 0;                                       // qword [obj+0xb8] = 0

    // 0xac9635: call 0xf590 on the stack description view (no control counter).
    const bool desc_inert = release_inert_view_f590(description);

    // 0xac963d..0xac964b.
    var.base.vptr = kBoolVarFinalVptr;                    // [obj] = 0x1594218
    var.value = value;                                    // byte [obj+0xc0] (1 at 301/299, 0 at 303)
    var.default_value = default_value;                    // byte [obj+0x68] (1 at 301/299, 0 at 303)

    // 0xac9652: call 0x20d60.
    out.registration = register_concmd_20d60(var.base, registry);

    // 0xac965a: call 0xf590 on the stack name view.
    const bool name_inert = release_inert_view_f590(name);
    out.views_inert = desc_inert && name_inert;

    // 0xac965f..0xac9670: __cxa_atexit(0x20b0, obj, &dso).
    out.atexit_recorded = registry.atexit_log.record(kBoolVarDtorRva, &var);

    out.ok = out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

BoolVarResult init_bool_var_vector(ConCmdBoolVar& var, const SharedStr& name,
                                   const SharedStr& description, std::uint64_t fn_vptr_tag,
                                   ConCmdRegistry& registry,
                                   SharedStrDeallocatorF590* name_deallocator,
                                   std::uint8_t value,
                                   std::uint8_t default_value) noexcept {
    // CPU-01-SLOT306-CATEGORY-VARS-01: identical construction to init_bool_var_static minus the
    // __cxa_atexit record. The sound-engine category vars (engine-init 0xa5d990 loop: 32 Mute +
    // 32 Filter `sndxCategory_*` bool vars at engine+0x8bd0/+0x72a8, stride 0xc8) live inside
    // engine-owned vectors; the engine teardown (0xa5cb30 -> the vector destructors) destroys
    // them, so the original never registers a per-object atexit dtor. Their formatted names are
    // OWNED SharedStrs: the construction acquires the view and the loop releases its stack
    // reference through the real 0xf590, so an owned name requires a deallocator here (a borrowed
    // name is an inert release and needs none).
    BoolVarResult out{};

    var.base.vptr = kBoolVarFinalVptr;
    init_concmd_base_20af0(var.base, name, description, 1);

    var.base.vptr = kBoolVarMidVptr;
    var.value_ref = &var.value;
    std::memset(var.zero69, 0, sizeof var.zero69);
    std::memset(var.zero70, 0, sizeof var.zero70);
    var.fn_payload = 0;
    var.fn_vptr = fn_vptr_tag;
    var.fn_aux = 0;

    const bool desc_inert = release_inert_view_f590(description);

    var.base.vptr = kBoolVarFinalVptr;
    var.value = value;
    var.default_value = default_value;

    out.registration = register_concmd_20d60(var.base, registry);

    bool name_ok = false;
    if (name.ctl == nullptr) {
        name_ok = release_inert_view_f590(name);          // borrowed: inert no-op release
    } else if (name_deallocator != nullptr) {              // owned: the observed real release
        release_shared_str_f590(const_cast<SharedStr&>(name), *name_deallocator);
        name_ok = true;
    }
    out.views_inert = desc_inert && name.ctl == nullptr && name_ok;

    out.atexit_recorded = false;   // vector-owned: no __cxa_atexit(0x20b0) record
    out.ok = out.registration.ok && desc_inert && name_ok;
    return out;
}

namespace {

// Shared by 0xb0e060 and 0xc300: the argument copy at +0x80 and the functor install through the
// lifted slot bodies. `vtbl` is the constant the class slots store (0x15d5960 / 0x1594318).
//   saved_aux = args.fn_aux
//   slot1 (clone)  tmp1 <- args.fn : tmp1.payload = args.fn_payload; tmp1.vptr = vtbl
//   slot0 (copy)   tmp2 <- tmp1    : tmp2.payload = tmp1.payload;    tmp2.vptr = vtbl
//   slot0 (copy)   tmp1 <- this.fn : tmp1.payload = this.fn_payload; tmp1.vptr = vtbl
//   slot0 (copy)   this.fn <- tmp2 : this.fn_payload = tmp2.payload; this.fn_vptr = vtbl
//   swap(this.fn_aux, saved_aux)
//   slot3 (bare ret) destroys tmp1: no effect.
void copy_args_and_install_fn(ConCmdRangedVar& var, const RangedVarArgs& args,
                              std::uint64_t vtbl) noexcept {
    var.args[1] = args.q[1];                // vmovups [this+0x88] <- [r8+8]
    var.args[2] = args.q[2];
    var.args[0] = args.q[0];                // [this+0x80] <- [r8]
    var.args[3] = args.q[3];                // [this+0x98] <- [r8+0x18]
    if (args.q[3] != 0) {
        var.args[0] = reinterpret_cast<std::uint64_t>(&var.args[0]);   // [this+0x80] = this+0x80
    }

    const std::uint64_t saved_aux = args.fn_aux;
    const std::uint64_t tmp1_payload = args.fn_payload;
    const std::uint64_t tmp2_payload = tmp1_payload;
    var.fn_payload = tmp2_payload;
    var.fn_vptr = vtbl;
    // The previous aux word lands in tmp1, which the bare-ret destroy slot discards.
    var.fn_aux = saved_aux;
}

}  // namespace

bool init_ranged_float_var_c300(ConCmdRangedVar& var, const SharedStr& name, void* value_ref,
                                float min_value, float max_value, std::uint32_t flags,
                                const RangedVarArgs& args, const SharedStr& description) noexcept {
    // Clone/copy/move/destroy bodies are identical for 0x15d57e0, 0x15d2040 and 0x1594318 (checker-pinned).
    if (args.fn_vptr != kRangedFloatArgFnVtbl && args.fn_vptr != kRangedFloatFnVtbl &&
        args.fn_vptr != kRangedFloatArgFnVtblSndx) {
        return false;
    }

    // 0xc336: call 0x20af0(this, name, description, flags).
    init_concmd_base_20af0(var.base, name, description, flags);

    // 0xc33b..0xc38e.
    var.base.vptr = kRangedFloatMidVptr;               // [this] = 0x1594118 + 0x10
    var.value_ref = value_ref;                         // [this+0x60] = rdx
    var.min_value[0] = 0;                              // byte [this+0x6c] = 0 (then overwritten)
    var.min_engaged = 1;                               // byte [this+0x70] = 1
    std::memcpy(var.min_value, &min_value, 4);         // dword [this+0x6c] = *min
    var.max_value[0] = 0;                              // byte [this+0x74] = 0 (then overwritten)
    var.max_engaged = 1;                               // byte [this+0x78] = 1
    std::memcpy(var.max_value, &max_value, 4);         // dword [this+0x74] = *max
    std::memset(var.args, 0, sizeof var.args);         // ymm [this+0x80] = 0
    var.fn_vptr = kRangedFloatFnVtbl;                  // [this+0xc0] = 0x1594318
    var.fn_payload = 0;                                // [this+0xb0] = 0
    var.fn_aux = 0;                                    // [this+0xc8] = 0

    copy_args_and_install_fn(var, args, kRangedFloatFnVtbl);
    return true;
}

bool init_ranged_var_b0e060(ConCmdRangedVar& var, const SharedStr& name, void* value_ref,
                            std::uint32_t flags, const RangedVarArgs& args,
                            const SharedStr& description) noexcept {
    // The functor vtable selects code through `call [vtbl+8]`; only 0x15d5960 is lifted.
    if (args.fn_vptr != kRangedEnumFnVtbl) return false;

    // 0xb0e08c: call 0x20af0(this, name, description, flags).
    init_concmd_base_20af0(var.base, name, description, flags);

    // 0xb0e091..0xb0e0d8.
    var.base.vptr = kRangedEnumMidVptr;     // [this] = 0x15d5870 + 0x10
    var.value_ref = value_ref;              // [this+0x60] = rdx (the value cell)
    var.min_value[0] = 0;                   // byte [this+0x6c] = 0
    var.min_engaged = 0;                    // byte [this+0x70] = 0
    var.max_value[0] = 0;                   // byte [this+0x74] = 0
    var.max_engaged = 0;                    // byte [this+0x78] = 0
    std::memset(var.args, 0, sizeof var.args);  // ymm [this+0x80] = 0
    var.fn_vptr = kRangedEnumFnVtbl;        // [this+0xc0] = 0x15d5960
    var.fn_payload = 0;                     // [this+0xb0] = 0
    var.fn_aux = 0;                         // [this+0xc8] = 0

    copy_args_and_install_fn(var, args, kRangedEnumFnVtbl);
    return true;
}

}  // namespace sotc
