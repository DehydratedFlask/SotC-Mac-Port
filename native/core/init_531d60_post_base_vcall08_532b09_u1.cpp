#include "init_531d60_post_base_vcall08_532b09_u1.hpp"
namespace sotc {
Init531d60PostBaseVcall08_532b09_U1Result init_531d60_post_base_vcall08_532b09_u1(
    const Init531d60PostBaseVcall08_532b09_U1Hooks& hooks) noexcept {
    Init531d60PostBaseVcall08_532b09_U1Result out{};
    if (hooks.stat_engine) {
        out.stat_query = stat_query_54b250(hooks.this_obj, 0, 2, *hooks.stat_engine);
        out.stat_query_ran = true;
    } else if (hooks.call_54b250) {
        hooks.call_54b250(hooks.this_obj, 0, 2);
    }
    ++out.vcall_54b250_count;
    out.ok = out.vcall_54b250_count == 1;
    return out;
}
}
