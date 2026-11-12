// CPU-01-HELPER-CALLEE-MODELS-04: see helper_49ca60_ranged_dword_var.hpp.
#include "helper_49ca60_ranged_dword_var.hpp"

#include <cstring>

namespace sotc {

RangedDwordVarResult helper_ranged_dword_var_49ca60(ConCmdRangedVar& var, RangedDwordVariant variant, const SharedStr& name, void* value_ref,
                                                    const std::uint8_t* min_value, const std::uint8_t* max_value, std::uint32_t flags,
                                                    const RangedVarArgs& args, const SharedStr& description) noexcept {
    RangedDwordVarResult out{};
    const std::uint64_t cls = ranged_dword_class(variant);
    // The clone slot `call [args.fn_vptr + 8]` exists only for the constructor's own class table.
    if (args.fn_vptr != cls) {
        out.refusal = HelperRefusal::unadmitted_arg_functor;
        return out;
    }
    if (min_value == nullptr || max_value == nullptr) {
        out.refusal = HelperRefusal::wrong_helper;
        return out;
    }

    // 0x49ca96: call 0x20af0(this, name, description, flags).
    init_concmd_base_20af0(var.base, name, description, flags);

    // 0x49ca9b..0x49caee.
    var.base.vptr = ranged_dword_vptr(variant);        // [this] = table + 0x10
    var.value_ref = value_ref;                         // [this+0x60] = rdx
    var.min_value[0] = 0;                              // byte [this+0x6c] = 0 (then overwritten)
    var.min_engaged = 1;                               // byte [this+0x70] = 1
    std::memcpy(var.min_value, min_value, 4);          // dword [this+0x6c] = *min
    var.max_value[0] = 0;                              // byte [this+0x74] = 0 (then overwritten)
    var.max_engaged = 1;                               // byte [this+0x78] = 1
    std::memcpy(var.max_value, max_value, 4);          // dword [this+0x74] = *max
    std::memset(var.args, 0, sizeof var.args);         // ymm [this+0x80] = 0
    var.fn_vptr = cls;                                 // [this+0xc0] = class
    var.fn_payload = 0;                                // [this+0xb0] = 0
    var.fn_aux = 0;                                    // [this+0xc8] = 0

    // 0x49caf9..0x49cb98: the argument copy and the functor install through the class slots, collapsed to the net state.
    install_ranged_fn_from_args(var, args, cls);
    out.ok = true;
    return out;
}

}  // namespace sotc
