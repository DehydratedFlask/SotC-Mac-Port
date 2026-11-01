#include "init_531d60_post_base_je_532bcc_532bc2_u1.hpp"

namespace sotc {

Init531d60PostBaseJe532bcc_532bc2_U1Result init_531d60_post_base_je_532bcc_532bc2_u1(
    const Init531d60PostBaseJe532bcc_532bc2_U1Cells& cells) noexcept {
    Init531d60PostBaseJe532bcc_532bc2_U1Result out{};

    if (!cells.r13_is_zero_known) {
        return out;
    }

    out.branch_outcome_modelled = true;
    out.branch_taken = cells.r13_is_zero;
    ++out.je_532bcc_count;

    // Next: fall-through @ 0x532bc4 or 0x532bcc — not modelled here.
    out.ok = (out.je_532bcc_count == 1);
    return out;
}

}  // namespace sotc
