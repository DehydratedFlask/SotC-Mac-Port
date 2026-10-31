#include "init_531d60_post_base_a52b70_u1.hpp"

namespace sotc {

Init531d60PostBaseA52b70U1Result init_531d60_post_base_a52b70_u1(
    const Init531d60PostBaseA52b70U1Hooks& hooks) noexcept {
    Init531d60PostBaseA52b70U1Result out{};

    const std::uint8_t esi = hooks.incoming_r14b;
    void* rdi = const_cast<void*>(hooks.incoming_r15);
    out.esi_arg = esi;
    out.rdi_arg = rdi;

    if (hooks.call_a52b70 != nullptr) {
        hooks.call_a52b70(rdi, esi);
    }
    ++out.call_a52b70_count;

    out.ok = (out.call_a52b70_count == 1);
    return out;
}

}  // namespace sotc
