#include "init_531d60_post_base_je_532bf8_532b50_u1.hpp"

namespace sotc {

Init531d60PostBaseJe532bf8_532b50_U1Result init_531d60_post_base_je_532bf8_532b50_u1(
    const Init531d60PostBaseJe532bf8_532b50_U1Cells& cells) noexcept {
    Init531d60PostBaseJe532bf8_532b50_U1Result out{};

    if (!cells.r12_is_zero_known) {
        return out;
    }

    out.branch_outcome_modelled = true;
    out.branch_taken = cells.r12_is_zero;
    ++out.je_532bf8_count;

    // Next: destroy fall-through @ 0x532b56 or 0x532bf8 block — not modelled here.
    out.ok = (out.je_532bf8_count == 1);
    return out;
}

}  // namespace sotc
