#include "init_531d60_post_base_je_532b98_532b7b_u1.hpp"

namespace sotc {

Init531d60PostBaseJe532b98_532b7b_U1Result init_531d60_post_base_je_532b98_532b7b_u1(
    const Init531d60PostBaseJe532b98_532b7b_U1Cells& cells) noexcept {
    Init531d60PostBaseJe532b98_532b7b_U1Result out{};

    if (!cells.pointers_equal_known) {
        return out;
    }

    out.branch_outcome_modelled = true;
    out.branch_taken = cells.pointers_equal;
    ++out.je_532b98_count;

    // Next: fall-through @ 0x532b7d or loop exit @ 0x532b98 — not modelled here.
    out.ok = (out.je_532b98_count == 1);
    return out;
}

}  // namespace sotc
