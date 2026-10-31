#include "init_531d60_post_base_continue_d.hpp"

#include "init_531d60_post_base_a473b0_u1.hpp"

namespace sotc {

Init531d60PostBaseContinueDResult init_531d60_post_base_continue_d(
    const Init531d60PostBaseContinueDHooks& hooks) noexcept {
    Init531d60PostBaseContinueDResult out{};

    if (!hooks.merge_a473b0_u1_ran) {
        Init531d60PostBaseA473b0U1Hooks u1{};
        u1.cell_32f79a8 = hooks.cell_32f79a8;
        u1.call_a473b0 = hooks.call_a473b0;
        u1.capture_services = hooks.capture_services;
        const Init531d60PostBaseA473b0U1Result u1r = init_531d60_post_base_a473b0_u1(u1);
        if (!u1r.ok) {
            return out;
        }
        out.call_count = u1r.call_a473b0_count;
        out.load_count = u1r.load_count;
        out.capture_native = u1r.capture_native;
        out.capture = u1r.capture;
    }

    out.ok = (out.unit_executed == kInit531d60PostBaseContinueDUnitCount) &&
             (out.call_count == 1) && (out.load_count <= 1);
    return out;
}

}  // namespace sotc
