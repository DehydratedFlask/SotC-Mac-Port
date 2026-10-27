#include "string_var_257a0.hpp"

#include <cstring>

#include "init_sub_3be930.hpp"

namespace sotc {

namespace {

// `lock inc qword [ctl]` when the control counter is present.
void copy_view_with_ref(SharedStr& dst, const SharedStr& src) noexcept {
    dst = src;
    if (src.ctl != nullptr) __atomic_fetch_add(src.ctl, 1, __ATOMIC_SEQ_CST);
}

// The desc view 0x257a0 builds on its stack: {0xf96388 -> "", 0, 0, 0}; the guest address holds a NUL byte.
constexpr char kEmptyDescription[] = "";

}  // namespace

bool init_string_var_inner_258f0(ConCmdStringVar& var, const SharedStr& name, void* value_ref, std::uint32_t flags,
                                 const RangedVarArgs& args, const SharedStr& description) noexcept {
    // The functor tag selects code through `call [tag + 8]`; only the lifted class is accepted.
    if (args.fn_vptr != kStringVarFnVtbl) return false;

    // 0x2591c: call 0x20af0(this, name, description, flags)
    init_concmd_base_20af0(var.base, name, description, flags);

    // 0x25921..0x25972, in this order.
    var.base.vptr = kStringVarInnerVptr;                  // [this] = 0x1594f58 + 0x10
    var.value_ref = value_ref;                            // [this+0x60] = r15 (rdx)
    init_slot_f180(&var.default_value);                   // 0xf180(this+0x68): empty view
    var.byte88 = 0;
    var.bytea8 = 0;
    var.byteb0 = 0;
    var.byted0 = 0;
    std::memset(var.args, 0, sizeof var.args);            // ymm [+0xd8] = 0
    var.fn_vptr = kStringVarFnVtbl;                       // [+0x118] = 0x1595068
    var.fn_payload = 0;                                   // [+0x108] = 0
    var.fn_aux = 0;                                       // [+0x120] = 0

    // 0x25988..0x259b7: the argument copy (q[1..2] as one xmm, q[0], q[3]; self pointer when q[3] != 0)
    var.args[1] = args.q[1];
    var.args[2] = args.q[2];
    var.args[0] = args.q[0];
    var.args[3] = args.q[3];
    if (args.q[3] != 0) var.args[0] = reinterpret_cast<std::uint64_t>(&var.args[0]);

    // 0x259ba..0x25a2d: functor install through the class 0x1595068 slots (copy / clone store the payload
    // at +0x10 and the class tag at +0x20), swap(this.aux, args.aux), bare-ret destroy of the temporary.
    const std::uint64_t saved_aux = args.fn_aux;
    var.fn_payload = args.fn_payload;
    var.fn_vptr = kStringVarFnVtbl;
    var.fn_aux = saved_aux;
    return true;
}

StringVarResult init_string_var_257a0(ConCmdStringVar& var, const SharedStr& name, const SharedStr& init_view,
                                      ConCmdRegistry& registry) noexcept {
    StringVarResult out{};

    // 0x257c6..0x25841: [this] = 0x1594ef0 (overwritten below), the stack argument block (zero q[4], payload 0,
    // functor class 0x1595068, aux 0) and the empty description view; call 0x258f0.
    var.base.vptr = kStringVarFinalVptr;
    RangedVarArgs args{};
    args.fn_vptr = kStringVarFnVtbl;
    const SharedStr description{reinterpret_cast<const std::uint8_t*>(kEmptyDescription), 0, 0, nullptr};
    if (!init_string_var_inner_258f0(var, name, &var.value, kStringVarFlags, args, description)) return out;

    // 0x25846: call 0xf590 on the description view; 0x2584e: call [functor class + 0x18] (bare ret 0x25aa0).
    const bool desc_inert = release_inert_view_f590(description);

    // 0x25859..0x25881: [this] = 0x1594ef0; value (+0x128) = init_view, lock inc [ctl]
    var.base.vptr = kStringVarFinalVptr;
    copy_view_with_ref(var.value, init_view);

    // 0x25888..0x258b8: if (this+0x68 != init_view) { 0xf590(this+0x68); default = init_view; lock inc }
    bool default_inert = true;
    if (&var.default_value != &init_view) {
        default_inert = release_inert_view_f590(var.default_value);
        copy_view_with_ref(var.default_value, init_view);
    }

    // 0x258bf: call 0x20d60
    out.registration = register_concmd_20d60(var.base, registry);
    out.views_inert = desc_inert && default_inert;
    out.ok = out.registration.ok && out.views_inert;
    return out;
}

DebugMenuTarget string_var_menu_target(const ConCmdStringVar& var) noexcept {
    DebugMenuTarget target{};
    target.object = &var;
    target.known = var.base.vptr == kStringVarFinalVptr;
    target.slot3 = target.known ? kStringVarSlot3Result : 0;
    target.slot6 = target.known ? kStringVarSlot6Result : 0;
    return target;
}

}  // namespace sotc
