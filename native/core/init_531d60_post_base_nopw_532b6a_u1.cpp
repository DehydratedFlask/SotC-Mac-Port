#include "init_531d60_post_base_nopw_532b6a_u1.hpp"

namespace sotc {

Init531d60PostBaseNopw_532b6a_U1Result init_531d60_post_base_nopw_532b6a_u1() noexcept {
    Init531d60PostBaseNopw_532b6a_U1Result out{};
    ++out.nopw_532b6a_count;

    // Next: E571 mov rbx,[r13+0x10] @ 0x532b70 — separate unit.
    out.ok = (out.nopw_532b6a_count == 1);
    return out;
}

}  // namespace sotc
