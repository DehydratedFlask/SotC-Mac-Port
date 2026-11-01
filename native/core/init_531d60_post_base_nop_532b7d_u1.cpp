#include "init_531d60_post_base_nop_532b7d_u1.hpp"

namespace sotc {

Init531d60PostBaseNop532b7d_U1Result init_531d60_post_base_nop_532b7d_u1(
    const Init531d60PostBaseNop532b7d_U1Cells& /*cells*/) noexcept {
    Init531d60PostBaseNop532b7d_U1Result out{};
    ++out.nop_532b7d_count;

    // Next: mov rax,[rbx+0x48] @ 0x532b80 — not modelled here.
    out.ok = (out.nop_532b7d_count == 1);
    return out;
}

}  // namespace sotc
