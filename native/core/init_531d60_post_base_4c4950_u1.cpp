#include "init_531d60_post_base_4c4950_u1.hpp"

namespace sotc {

Init531d60PostBase4c4950U1Result init_531d60_post_base_4c4950_u1(
    const Init531d60PostBase4c4950U1Hooks& hooks) noexcept {
    Init531d60PostBase4c4950U1Result out{};

    const std::uint8_t bl = static_cast<std::uint8_t>(hooks.incoming_eax_from_1e4c0_u1 & 0xFF);
    const std::uint8_t edi_arg = static_cast<std::uint8_t>(bl ^ 1U);
    out.edi_arg = edi_arg;
    out.edi_known = hooks.incoming_eax_known;

    if (hooks.call_4c4950 != nullptr) {
        hooks.call_4c4950(edi_arg);
    }
    ++out.call_4c4950_count;

    out.ok = (out.call_4c4950_count == 1);
    return out;
}

}  // namespace sotc
