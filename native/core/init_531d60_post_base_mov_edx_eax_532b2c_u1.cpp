#include "init_531d60_post_base_mov_edx_eax_532b2c_u1.hpp"

namespace sotc {

Init531d60PostBaseMovEdxEax_532b2c_U1Result init_531d60_post_base_mov_edx_eax_532b2c_u1(
    const Init531d60PostBaseMovEdxEax_532b2c_U1Cells& cells) noexcept {
    Init531d60PostBaseMovEdxEax_532b2c_U1Result out{};

    out.edx = cells.eax_in;
    if (cells.edx_out != nullptr) {
        *cells.edx_out = out.edx;
    }
    ++out.mov_edx_eax_count;

    // Next: call 0x52a220 @ 0x532b2e — not modelled in this unit.
    out.ok = (out.mov_edx_eax_count == 1);
    return out;
}

}  // namespace sotc
