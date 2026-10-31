#include "init_531d60_post_base_continue_c_after_merge.hpp"

namespace sotc {

InitContinueCAfterMergeResult run_init_continue_c_after_merge(
    const Init531d60PostBaseContinueCHooks& c_hooks,
    const Init531d60PostBaseContinueDHooks& d_hooks) noexcept {
    InitContinueCAfterMergeResult out{};
    out.cont_c = init_531d60_post_base_continue_c(c_hooks);
    if (!out.cont_c.ok) {
        return out;
    }
    if (out.cont_c.merge_branch_outcome_modelled && out.cont_c.merge_branch_taken) {
        return out;
    }
    out.fallthrough = init_531d60_post_base_continue_d(d_hooks);
    out.ran_fallthrough = true;
    return out;
}

}  // namespace sotc
