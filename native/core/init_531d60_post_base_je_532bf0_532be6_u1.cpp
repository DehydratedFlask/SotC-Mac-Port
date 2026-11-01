#include "init_531d60_post_base_je_532bf0_532be6_u1.hpp"

namespace sotc {

Init531d60PostBaseJe532bf0_532be6_U1Result init_531d60_post_base_je_532bf0_532be6_u1(
    const Init531d60PostBaseJe532bf0_532be6_U1Cells& cells) noexcept {
    Init531d60PostBaseJe532bf0_532be6_U1Result out{};

    out.rdi_is_zero = (cells.rdi_in == nullptr);
    out.branch_outcome_modelled = true;
    out.branch_taken = out.rdi_is_zero;
    ++out.je_532bf0_532be6_count;

    // Next: call @ 0x532beb or fall-through target @ 0x532bf0 — not modelled here.
    out.ok = (out.je_532bf0_532be6_count == 1);
    return out;
}

}  // namespace sotc
