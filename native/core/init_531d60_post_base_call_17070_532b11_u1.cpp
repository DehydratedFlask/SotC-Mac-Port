#include "init_531d60_post_base_call_17070_532b11_u1.hpp"

namespace sotc {

Init531d60PostBaseCall17070_532b11_U1Result init_531d60_post_base_call_17070_532b11_u1(
    const Init531d60PostBaseCall17070_532b11_U1Hooks& hooks) noexcept {
    Init531d60PostBaseCall17070_532b11_U1Result out{};
    if (hooks.call_17070 != nullptr) {
        hooks.call_17070();
    }
    ++out.call_17070_count;
    out.ok = (out.call_17070_count == 1);
    return out;
}

}  // namespace sotc
