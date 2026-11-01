#include "init_531d60_post_base_je_532bdb_532bd4_u1.hpp"

namespace sotc {

Init531d60PostBaseJe532bdb_532bd4_U1Result init_531d60_post_base_je_532bdb_532bd4_u1(
    const Init531d60PostBaseJe532bdb_532bd4_U1Cells& cells) noexcept {
    Init531d60PostBaseJe532bdb_532bd4_U1Result out{};

    if (!cells.rdi_is_zero_known) {
        return out;
    }

    out.branch_outcome_modelled = true;
    out.branch_taken = cells.rdi_is_zero;
    ++out.je_532bdb_count;

    // Next: fall-through call @ 0x532bd6 or epilogue @ 0x532bdb — not modelled here.
    out.ok = (out.je_532bdb_count == 1);
    return out;
}

}  // namespace sotc
