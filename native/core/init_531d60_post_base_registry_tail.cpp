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
    if (!hooks.merge_call_17070_532b11_u1_ran) {
        Init531d60PostBaseCall17070_532b11_U1Hooks call_hooks{};
        call_hooks.call_17070 = hooks.call_17070;
        const Init531d60PostBaseCall17070_532b11_U1Result call =
            init_531d60_post_base_call_17070_532b11_u1(call_hooks);
        if (!call.ok) {
            return out;
        }
        out.call_17070_count = call.call_17070_count;
        out.unit_counts[static_cast<std::size_t>(RegistryTailUnit::call_17070_532b11_u1)] = 1;
        ++out.unit_executed;
    }
    out.end_rva = kInit531d60PostBaseRegistryTailEndRva;
    const int expected_mov = hooks.merge_mov_edi_98_532b0c_u1_ran ? 0 : 1;
    const int expected_call = hooks.merge_call_17070_532b11_u1_ran ? 0 : 1;
    const int expected_units =
        hooks.merge_call_17070_532b11_u1_ran ? 0 : kInit531d60PostBaseRegistryTailUnitCount;
    out.ok = (out.unit_executed == expected_units) && (out.mov_edi_count == expected_mov) &&
             (out.call_17070_count == expected_call);
    return out;
}

}  // namespace sotc
