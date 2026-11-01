#include "init_531d60_post_base_je_532bba_532b68_u1.hpp"

namespace sotc {

Init531d60PostBaseJe532bba_532b68_U1Result init_531d60_post_base_je_532bba_532b68_u1(
    const Init531d60PostBaseJe532bba_532b68_U1Cells& cells) noexcept {
    Init531d60PostBaseJe532bba_532b68_U1Result out{};

    if (!cells.pointers_equal_known) {
        return out;
    }

    out.branch_outcome_modelled = true;
    out.branch_taken = cells.pointers_equal;
    ++out.je_532bba_count;

    // Next: fall-through @ 0x532b6a or 0x532bba destroy — not modelled here.
    out.ok = (out.je_532bba_count == 1);
    return out;
}

}  // namespace sotc
