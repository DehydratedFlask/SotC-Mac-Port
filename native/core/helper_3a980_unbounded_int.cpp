#include "helper_3a980_unbounded_int.hpp"

namespace sotc {

HelperCtorResult helper_3a980_unbounded_int(ConCmdRangedVar& var, const SharedStr& name, std::uint32_t flags,
                                            const RangedVarArgs& block, const SharedStr& description) noexcept {
    HelperCtorResult out{};
    if (!is_int_functor_tag(block.fn_vptr)) {
        out.refusal = HelperRefusal::unadmitted_arg_functor;   // nothing written
        return out;
    }
    RangedVarArgs lifted = block;
    lifted.fn_vptr = kUnboundedIntFnTable;   // 0x15944a0: the class every family clone slot stores
    if (!init_unbounded_int_var_worker_3a980(var, name, flags, lifted, description)) {
        out.refusal = HelperRefusal::unadmitted_arg_functor;   // defensive: delegate disagreed
        return out;
    }
    out.arg_destroy_ok = true;   // [tag + 0x18] == 0xc770 (bare ret)
    out.ok = true;
    return out;
}

}  // namespace sotc
