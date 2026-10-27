#include "ctor_shapes_343_356.hpp"

#include <cstring>

#include "init_sub_3be930.hpp"

namespace sotc {

namespace {

constexpr std::uint64_t kBareRetDestroyFunctors[] = {
    kRangedFloatArgFnVtbl,        // 0x15d57e0
    kRangedFloatFnVtbl,           // 0x1594318
    kRangedFloatArgFnVtblSndx,    // 0x15d2040
    kFloatArgFnVtbl_347,          // 0x15ce318
    kFloatArgFnVtbl_350,          // 0x15ce260
    kFloatArgFnVtbl_353B,         // 0x15ce1c0
    kUnboundedFloatInitialFnVtbl  // 0x1596878 (the functor class 0x6aa00 stores first)
};

// 0xc300 / 0x6aa00 tail: the argument copy and the functor clone/copy/copy/copy/swap sequence. Every
// known functor vtable has copy/clone slots that store 0x1594318 and a payload-copying move slot, and a
// bare-ret destroy slot, so the net effect is the stores below (identical to concmd_vars.cpp).
void copy_args_and_install_fn(ConCmdRangedVar& var, const RangedVarArgs& args) noexcept {
    var.args[1] = args.q[1];                // vmovups [this+0x88] <- [r8+8]
    var.args[2] = args.q[2];
    var.args[0] = args.q[0];                // [this+0x80] <- [r8]
    var.args[3] = args.q[3];                // [this+0x98] <- [r8+0x18]
    if (args.q[3] != 0) {
        var.args[0] = reinterpret_cast<std::uint64_t>(&var.args[0]);   // [this+0x80] = this+0x80
    }
    const std::uint64_t saved_aux = args.fn_aux;
    var.fn_payload = args.fn_payload;
    var.fn_vptr = kRangedFloatFnVtbl;       // every copy/clone slot stores 0x1594318
    var.fn_aux = saved_aux;
}

SharedStr borrowed_view(const char* text) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text),
                     static_cast<std::int64_t>(std::strlen(text)), 0, nullptr};
}

}  // namespace

bool is_known_float_arg_functor(std::uint64_t vtbl) noexcept {
    for (const std::uint64_t v : kBareRetDestroyFunctors) {
        if (v == vtbl) return true;
    }
    return false;
}

bool init_bounded_float_var_c300(ConCmdRangedVar& var, const SharedStr& name, void* value_ref, float min_value,
                                 float max_value, std::uint32_t flags, const RangedVarArgs& args,
                                 const SharedStr& description) noexcept {
    if (!is_known_float_arg_functor(args.fn_vptr)) return false;

    init_concmd_base_20af0(var.base, name, description, flags);   // 0xc336
    var.base.vptr = kRangedFloatMidVptr;                          // [this] = 0x1594118 + 0x10
    var.value_ref = value_ref;                                    // [this+0x60]
    var.min_value[0] = 0;
    var.min_engaged = 1;                                          // byte [this+0x70] = 1
    std::memcpy(var.min_value, &min_value, 4);
    var.max_value[0] = 0;
    var.max_engaged = 1;                                          // byte [this+0x78] = 1
    std::memcpy(var.max_value, &max_value, 4);
    std::memset(var.args, 0, sizeof var.args);                    // ymm [this+0x80] = 0
    var.fn_vptr = kRangedFloatFnVtbl;
    var.fn_payload = 0;
    var.fn_aux = 0;
    copy_args_and_install_fn(var, args);
    return true;
}

bool init_unbounded_float_var_6aa00(ConCmdRangedVar& var, const SharedStr& name, void* value_ref,
                                    std::uint32_t flags, const RangedVarArgs& args,
                                    const SharedStr& description) noexcept {
    if (!is_known_float_arg_functor(args.fn_vptr)) return false;

    init_concmd_base_20af0(var.base, name, description, flags);   // 0x6aa2c: call 0x20af0
    var.base.vptr = kUnboundedFloatMidVptr;                       // 0x6aa31..0x6aa47: 0x1594118 + 0x10
    var.value_ref = value_ref;                                    // 0x6aa4a: [this+0x60] = rdx
    var.min_value[0] = 0;                                         // 0x6aa4e byte [this+0x6c] = 0
    var.min_engaged = 0;                                          // 0x6aa52 byte [this+0x70] = 0
    var.max_value[0] = 0;                                         // 0x6aa56 byte [this+0x74] = 0
    var.max_engaged = 0;                                          // 0x6aa5a byte [this+0x78] = 0
    std::memset(var.args, 0, sizeof var.args);                    // 0x6aa5e ymm [this+0x80] = 0
    var.fn_vptr = kUnboundedFloatInitialFnVtbl;                   // 0x6aa66 [this+0xc0] = 0x1596878
    var.fn_payload = 0;                                           // 0x6aa6d
    var.fn_aux = 0;                                               // 0x6aa78
    copy_args_and_install_fn(var, args);                          // 0x6aa83..0x6ab21
    return true;
}

FloatVarResult init_float_var_static(ConCmdRangedVar& var, const FloatVarSpec& spec,
                                     ConCmdRegistry& registry) noexcept {
    FloatVarResult out{};
    const SharedStr name = borrowed_view(spec.name);
    const SharedStr description = borrowed_view(spec.description);
    RangedVarArgs args{};                        // four zero qwords, payload 0, aux 0
    args.fn_vptr = spec.arg_fn_vtbl;             // caller functor {0, vtbl, 0}

    var.base.vptr = kRangedFloatFinalVptr;       // the site pre-stores 0x15942a0 + 0x10; the ctor overwrites it
    if (spec.bounded) {
        float lo = 0.0f;
        float hi = 0.0f;
        std::memcpy(&lo, &spec.lo_bits, 4);
        std::memcpy(&hi, &spec.hi_bits, 4);
        out.ctor_ok = init_bounded_float_var_c300(var, name, &var.value, lo, hi, spec.flags, args, description);
    } else {
        out.ctor_ok = init_unbounded_float_var_6aa00(var, name, &var.value, spec.flags, args, description);
    }
    if (!out.ctor_ok) return out;

    // f590(description): both views are borrowed (ctl == nullptr), so the release is an exact no-op.
    const bool desc_inert = release_inert_view_f590(description);
    // `call [argfn + 0x18]`: the bare-ret destroy slot of the caller functor.
    out.functor_destroy_ok = is_known_float_arg_functor(args.fn_vptr);
    var.base.vptr = kRangedFloatFinalVptr;       // [this] = 0x15942a0 + 0x10
    var.value = spec.default_bits;               // dword [this+0xd0]
    var.default_value = spec.default_bits;       // dword [this+0x68]
    out.registration = register_concmd_20d60(var.base, registry);
    const bool name_inert = release_inert_view_f590(name);
    out.views_inert = desc_inert && name_inert;
    out.atexit_recorded = registry.atexit_log.record(kRangedFloatDtorRva, &var);
    out.ok = out.ctor_ok && out.functor_destroy_ok && out.registration.ok && out.views_inert &&
             out.atexit_recorded;
    return out;
}

bool init_menu_toggle_from_var_26db0(DebugMenuItem& item, const SharedStr& parent, const ConCmdBoolVar& target,
                                     std::uint32_t flags) noexcept {
    // The two `call [vptr + 0x18/0x30]` run unlifted code for any class but the lifted bool class.
    const DebugMenuTarget t = debug_menu_target_for(target);
    if (!t.known) return false;

    item.link.self = nullptr;              // 0x26dc3 [this] = 0
    item.link.prev = &item.link;           // 0x26dca [this+8] = this
    item.link.next = &item.link;           // 0x26dce [this+0x10] = this
    item.link.owner = nullptr;             // 0x26dd2
    item.registered = 0;                   // 0x26dda byte [this+0x20] = 0
    item.label = parent;                   // 0x26dde..0x26dfd: [+0x28..0x48) = *parent (+ lock inc ctl)
    concmd_acquire_view(parent);
    item.kind = kDebugMenuItemKind;        // 0x26e00 dword [this+0x48] = 2
    item.text = target.base.name;          // 0x26e07..0x26e27: [+0x50..0x70) = target->name (+ lock inc ctl)
    concmd_acquire_view(target.base.name);
    init_slot_f180(&item.empty);           // 0x26e2f call 0xf180(this+0x70)
    item.target = &target;                 // 0x26e34
    item.range_min = 1.0f;                 // 0x26e3b
    item.range_max = 10.0f;                // 0x26e45
    item.flags = flags;                    // 0x26e4f
    if (t.slot3 == 1 && t.slot6 == 3) {    // 0x26e5c..0x26e7c
        item.range_min = 0.1f;
        item.range_max = 0.5f;
    }
    return true;                           // 0x26e9a: jmp 0x268b0 (the caller registers)
}

bool init_zero_vector24(ZeroVector24& cell, ConCmdRegistry& registry) noexcept {
    cell.begin = 0;
    cell.end = 0;
    cell.cap = 0;
    return registry.atexit_log.record(kZeroVectorDtorRva, &cell);
}

void init_flag_scale_cell(FlagScaleCell& cell) noexcept {
    cell.flag_scale = kFlagScaleQword;
    std::atomic_ref<std::int32_t>(cell.atomic_word).store(0, std::memory_order_seq_cst);
}

BoolVarResult init_bool_var_site(ConCmdBoolVar& var, const char* name, std::uint64_t fn_tag,
                                 ConCmdRegistry& registry, std::uint8_t value,
                                 std::uint8_t default_value) noexcept {
    const SharedStr n = borrowed_view(name);
    const SharedStr d = borrowed_view("");
    return init_bool_var_static(var, n, d, fn_tag, registry, value, default_value);
}

MenuSiteResult init_toggle_site_26ea0(DebugMenuItem& item, const ConCmdBoolVar& target, const char* label,
                                      const char* text, ConCmdRegistry& registry, DebugMenuRegistry& menu) noexcept {
    MenuSiteResult out{};
    const SharedStr l = borrowed_view(label);
    const SharedStr t = borrowed_view(text);
    out.ctor_ok = init_debug_menu_item_26ea0(item, l, t, debug_menu_target_for(target), kL4MenuItemFlags);
    if (!out.ctor_ok) return out;
    out.registration = register_debug_menu_item_268b0(item, menu, registry.atexit_log);
    out.views_inert = release_inert_view_f590(t) && release_inert_view_f590(l);   // text first, then label
    out.atexit_recorded = registry.atexit_log.record(kDebugMenuItemDtorRva, &item);
    out.ok = out.ctor_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

MenuSiteResult init_toggle_site_26db0(DebugMenuItem& item, const ConCmdBoolVar& target, const char* parent,
                                      ConCmdRegistry& registry, DebugMenuRegistry& menu) noexcept {
    MenuSiteResult out{};
    const SharedStr p = borrowed_view(parent);
    out.ctor_ok = init_menu_toggle_from_var_26db0(item, p, target, kL4MenuItemFlags);
    if (!out.ctor_ok) return out;
    out.registration = register_debug_menu_item_268b0(item, menu, registry.atexit_log);
    out.views_inert = release_inert_view_f590(p);
    out.atexit_recorded = registry.atexit_log.record(kDebugMenuItemDtorRva, &item);
    out.ok = out.ctor_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

}  // namespace sotc
