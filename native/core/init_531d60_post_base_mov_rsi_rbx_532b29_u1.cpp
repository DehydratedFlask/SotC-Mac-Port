#include "init_531d60_post_base_mov_rsi_rbx_532b29_u1.hpp"

namespace sotc {

Init531d60PostBaseMovRsiRbx_532b29_U1Result init_531d60_post_base_mov_rsi_rbx_532b29_u1(
    const Init531d60PostBaseMovRsiRbx_532b29_U1Cells& cells) noexcept {
    Init531d60PostBaseMovRsiRbx_532b29_U1Result out{};

    void* rsi = const_cast<void*>(cells.rbx_in);
    out.rsi = rsi;
    if (cells.rsi_out != nullptr) {
        *cells.rsi_out = rsi;
    }
    ++out.mov_rsi_rbx_count;

    // Next: mov edx,eax @ 0x532b2c — peeled as registry_body unit E556.
    out.ok = (out.mov_rsi_rbx_count == 1);
    return out;
}

}  // namespace sotc
