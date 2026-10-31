#include "init_531d60_post_base_continue_b.hpp"

namespace sotc {
namespace {

int& slot(Init531d60PostBaseContinueBResult& out, ContinueBUnit u) noexcept {
    return out.unit_counts[static_cast<int>(u)];
}

void pin(Init531d60PostBaseContinueBResult& out, ContinueBUnit u) noexcept {
    ++slot(out, u);
    ++out.unit_executed;
}

}  // namespace

Init531d60PostBaseContinueBResult init_531d60_post_base_continue_b(
    const Init531d60PostBaseContinueBHooks& hooks) noexcept {
    Init531d60PostBaseContinueBResult out{};

    const int eax = hooks.incoming_eax_from_1e4c0_u1;
    out.eax_from_1e4c0 = eax;
    out.eax_known = hooks.incoming_eax_known;

    pin(out, ContinueBUnit::mov_ebx_eax);

    const std::uint8_t r12b = static_cast<std::uint8_t>(eax & 0xFF);
    pin(out, ContinueBUnit::mov_r12b_bl);

    const std::uint8_t edi_arg = r12b ^ 1;
    pin(out, ContinueBUnit::xor_r12b_1);
    ++out.alu_count;

    pin(out, ContinueBUnit::movzx_edi_r12b);

    if (hooks.call_4c4950 != nullptr) {
        hooks.call_4c4950(edi_arg);
    }
    pin(out, ContinueBUnit::call_4c4950);
    ++out.call_count;

    if (hooks.cell_32f84f8 != nullptr) {
        (void)*hooks.cell_32f84f8;
        ++out.load_count;
    }
    pin(out, ContinueBUnit::mov_rdi_r13_load);

    pin(out, ContinueBUnit::test_bl_bl);
    ++out.alu_count;

    pin(out, ContinueBUnit::je_5328e0);
    ++out.branch_boundary_count;
    out.bl = static_cast<std::uint8_t>(eax & 0xFF);
    const bool bl_is_boolean = out.eax_known && (eax == 0 || eax == 1);
    out.branch_outcome_modelled = bl_is_boolean;
    out.branch_taken = bl_is_boolean && (out.bl == 0);

    out.ok = (out.unit_executed == kInit531d60PostBaseContinueBUnitCount) &&
             (out.call_count == 1) && (out.alu_count == 2) && (out.load_count <= 1) &&
             (out.branch_boundary_count == 1) && (out.unit_counts[0] == 1) &&
             (out.unit_counts[kInit531d60PostBaseContinueBUnitCount - 1] == 1);
    return out;
}

}  // namespace sotc
