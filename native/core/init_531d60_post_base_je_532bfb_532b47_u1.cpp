#include "init_531d60_post_base_je_532bfb_532b47_u1.hpp"

namespace sotc {

Init531d60PostBaseJe532bfb_532b47_U1Result init_531d60_post_base_je_532bfb_532b47_u1(
    const Init531d60PostBaseJe532bfb_532b47_U1Cells& cells) noexcept {
    Init531d60PostBaseJe532bfb_532b47_U1Result out{};

    if (!cells.pointers_equal_known) {
        return out;
    }

    out.branch_outcome_modelled = true;
    out.branch_taken = cells.pointers_equal;
    ++out.je_532bfb_count;

    // Next: test r12,r12 @ 0x532b4d (fall-through) or 0x532bfb block — not modelled here.
    out.ok = (out.je_532bfb_count == 1);
    return out;
}

}  // namespace sotc
