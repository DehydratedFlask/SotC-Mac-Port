#include "init_531d60_post_base_cmp_r15_rbx_532b8f_u1.hpp"

namespace sotc {

Init531d60PostBaseCmpR15Rbx_532b8f_U1Result init_531d60_post_base_cmp_r15_rbx_532b8f_u1(
    const Init531d60PostBaseCmpR15Rbx_532b8f_U1Cells& cells) noexcept {
    Init531d60PostBaseCmpR15Rbx_532b8f_U1Result out{};

    out.pointers_equal = (cells.r15_in == cells.rbx_in);
    ++out.cmp_r15_rbx_count;

    // Next: jne @ 0x532b92 — NOT modelled here.
    out.ok = (out.cmp_r15_rbx_count == 1);
    return out;
}

}  // namespace sotc
