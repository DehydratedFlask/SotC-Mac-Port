#include "init_531d60_post_base_registry_tail.hpp"

namespace sotc {

Init531d60PostBaseRegistryTailResult init_531d60_post_base_registry_tail(
    const Init531d60PostBaseRegistryTailHooks& hooks) noexcept {
    Init531d60PostBaseRegistryTailResult out{};
    if (!hooks.merge_mov_edi_98_532b0c_u1_ran) {
        const Init531d60PostBaseMovEdi98_532b0c_U1Result mov =
            init_531d60_post_base_mov_edi_98_532b0c_u1();
        if (!mov.ok) {
            return out;
        }
        out.mov_edi_count = mov.mov_edi_count;
    }
    out.ok = (out.unit_executed == kInit531d60PostBaseRegistryTailUnitCount) &&
             (out.mov_edi_count == (hooks.merge_mov_edi_98_532b0c_u1_ran ? 0 : 1));
    return out;
}

}  // namespace sotc
