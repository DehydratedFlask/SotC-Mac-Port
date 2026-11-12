#include "helper_c580_ranged_int.hpp"

namespace sotc {

HelperCtorResult helper_c580_ranged_int(ConCmdRangedVar& var, const SharedStr& name, void* value_ref,
                                        std::int32_t min_value, std::int32_t max_value, std::uint32_t flags,
                                        const RangedVarArgs& args, const SharedStr& description) noexcept {
    HelperCtorResult out{};
    if (!is_int_functor_tag(args.fn_vptr)) {
        out.refusal = HelperRefusal::unadmitted_arg_functor;   // nothing written
        return out;
    }
    // Every family clone slot (0xc740) stores {payload, class 0x15944a0}; the delegate's lifted-tag set
    // contains 0x15944a0, so the normalised copy reproduces the exact stores.
    RangedVarArgs lifted = args;
    lifted.fn_vptr = kRangedIntFnVtbl;
    if (!init_ranged_int_var_c580(var, name, value_ref, min_value, max_value, flags, lifted, description)) {
        out.refusal = HelperRefusal::unadmitted_arg_functor;   // defensive: delegate disagreed
        return out;
    }
    out.arg_destroy_ok = true;   // [tag + 0x18] == 0xc770 (bare ret) for every family table
    out.ok = true;
    return out;
}

}  // namespace sotc
