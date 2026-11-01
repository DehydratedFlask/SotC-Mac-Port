#include "init_531d60_post_base_je_532ba5_532b9b_u1.hpp"

namespace sotc {

Init531d60PostBaseJe532ba5_532b9b_U1Result init_531d60_post_base_je_532ba5_532b9b_u1(
    const Init531d60PostBaseJe532ba5_532b9b_U1Cells& cells) noexcept {
    Init531d60PostBaseJe532ba5_532b9b_U1Result out{};

    if (!cells.rbx_is_zero_known) {
        return out;
    }

    out.branch_outcome_modelled = true;
    out.branch_taken = cells.rbx_is_zero;
    ++out.je_532ba5_count;

    // Next: fall-through mov @ 0x532b9d (E585) or epilogue @ 0x532ba5 — not modelled here.
    out.ok = (out.je_532ba5_count == 1);
    return out;
}

}  // namespace sotc
