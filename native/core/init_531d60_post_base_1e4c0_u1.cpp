#include "init_531d60_post_base_1e4c0_u1.hpp"

namespace sotc {

Init531d60PostBase1e4c0U1Result init_531d60_post_base_1e4c0_u1(
    const Init531d60PostBase1e4c0U1Hooks& hooks) noexcept {
    Init531d60PostBase1e4c0U1Result out{};

    void* rdi = const_cast<void*>(hooks.incoming_rax_standin);
    int eax = 0;
    if (hooks.call_1e4c0 != nullptr) {
        eax = hooks.call_1e4c0(rdi);
        out.eax_known = true;
    }
    ++out.call_1e4c0_count;

    out.eax_return = eax;
    out.ok = (out.call_1e4c0_count == 1);
    return out;
}

}  // namespace sotc
