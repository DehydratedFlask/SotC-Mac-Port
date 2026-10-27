#include "ranged_int_var_c580.hpp"

#include <cstring>

namespace sotc {

bool init_ranged_int_var_c580(ConCmdRangedVar& var, const SharedStr& name, void* value_ref, std::int32_t min_value,
                              std::int32_t max_value, std::uint32_t flags, const RangedVarArgs& args,
                              const SharedStr& description) noexcept {
    // The functor tag selects code through `call [tag + 8]`; only the two lifted tags are accepted.
    if (args.fn_vptr != kRangedIntArgFnVtbl && args.fn_vptr != kRangedIntFnVtbl) return false;

    // 0xc5b6: call 0x20af0(this, name, description, flags)
    init_concmd_base_20af0(var.base, name, description, flags);

    // 0xc5bb..0xc61f
    var.base.vptr = kRangedIntMidVptr;                    // [this] = 0x1594190 + 0x10
    var.value_ref = value_ref;                            // [this+0x60] = rdx
    var.min_value[0] = 0;                                 // byte [this+0x6c] = 0 (then overwritten)
    var.min_engaged = 1;                                  // byte [this+0x70] = 1
    std::memcpy(var.min_value, &min_value, 4);            // dword [this+0x6c] = *min
    var.max_value[0] = 0;                                 // byte [this+0x74] = 0 (then overwritten)
    var.max_engaged = 1;                                  // byte [this+0x78] = 1
    std::memcpy(var.max_value, &max_value, 4);            // dword [this+0x74] = *max
    std::memset(var.args, 0, sizeof var.args);            // ymm [this+0x80] = 0
    var.fn_vptr = kRangedIntFnVtbl;                       // [this+0xc0] = 0x15944a0
    var.fn_payload = 0;                                   // [this+0xb0] = 0
    var.fn_aux = 0;                                       // [this+0xc8] = 0

    // 0xc619..0xc698: the argument block copy at +0x80 (q[1..2] first as one xmm, then q[0], then q[3])
    var.args[1] = args.q[1];
    var.args[2] = args.q[2];
    var.args[0] = args.q[0];
    var.args[3] = args.q[3];
    if (args.q[3] != 0) var.args[0] = reinterpret_cast<std::uint64_t>(&var.args[0]);   // [this+0x80] = this+0x80

    // 0xc64c..0xc6b8: functor install through the lifted slot bodies 0xc720 / 0xc740 (store payload and
    // the class tag 0x15944a0), then swap(this.fn_aux, args.fn_aux); the bare-ret destroy (0xc770) of
    // the temporary has no effect.
    const std::uint64_t saved_aux = args.fn_aux;
    var.fn_payload = args.fn_payload;
    var.fn_vptr = kRangedIntFnVtbl;
    var.fn_aux = saved_aux;
    return true;
}

DebugMenuTarget ranged_int_c580_menu_target(const ConCmdRangedVar& var) noexcept {
    DebugMenuTarget target{};
    target.object = &var;
    target.known = var.base.vptr == kRangedIntFinalVptr;
    target.slot3 = target.known ? kRangedIntSlot3Result : 0;
    target.slot6 = target.known ? kRangedIntSlot6Result : 0;
    return target;
}

}  // namespace sotc
