#include "init_531d60_post_base_jne_532b80_532b92_u1.hpp"

namespace sotc {

Init531d60PostBaseJne532b80_532b92_U1Result init_531d60_post_base_jne_532b80_532b92_u1(
    const Init531d60PostBaseJne532b80_532b92_U1Cells& cells) noexcept {
    Init531d60PostBaseJne532b80_532b92_U1Result out{};

    if (!cells.pointers_equal_known) {
        return out;
    }

    out.branch_outcome_modelled = true;
    out.branch_taken = !cells.pointers_equal;
    ++out.jne_532b80_count;

    // Next: fall-through mov @ 0x532b94 (E582) or loop @ 0x532b80 — re-entry not modelled here.
    out.ok = (out.jne_532b80_count == 1);
    return out;
}

}  // namespace sotc
