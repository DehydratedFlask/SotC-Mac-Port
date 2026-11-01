#include "init_531d60_post_base_cmp_r13_r14_532ba9_u1.hpp"

namespace sotc {

Init531d60PostBaseCmpR13R14_532ba9_U1Result init_531d60_post_base_cmp_r13_r14_532ba9_u1(
    const Init531d60PostBaseCmpR13R14_532ba9_U1Cells& cells) noexcept {
    Init531d60PostBaseCmpR13R14_532ba9_U1Result out{};

    out.pointers_equal = (cells.r13_in == cells.r14_in);
    ++out.cmp_r13_r14_532ba9_count;

    // Next: jne @ 0x532bac — not modelled in this unit.
    out.ok = (out.cmp_r13_r14_532ba9_count == 1);
    return out;
}

}  // namespace sotc
