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

    // 0x53277f xor esi,esi
    pin(out, ContinueBUnit::xor_esi_esi);
    ++out.alu_count;

    // 0x532781 xor edx,edx
    pin(out, ContinueBUnit::xor_edx_edx);
    ++out.alu_count;

    // 0x532783 mov ecx,1 — constant-only pin.
    pin(out, ContinueBUnit::mov_ecx_1);

    // 0x532788 mov rdi,rax — rax is the (non-zero) return from continue_a's
    // call 0x1fc80, NOT modelled. Host stand-in supplies the pointer.
    const void* rdi_1e4c0 = hooks.incoming_rax_standin;
    pin(out, ContinueBUnit::mov_rdi_rax);

    // 0x53278b xor r14d,r14d
    pin(out, ContinueBUnit::xor_r14d_r14d);
    ++out.alu_count;

    // 0x53278e call 0x1e4c0 (body NOT modelled).
    int eax = 0;
    if (hooks.call_1e4c0 != nullptr) {
        eax = hooks.call_1e4c0(const_cast<void*>(rdi_1e4c0));
    }
    pin(out, ContinueBUnit::call_1e4c0);
    ++out.call_count;
    out.eax_from_1e4c0 = eax;
    out.eax_known = (hooks.call_1e4c0 != nullptr);

    // 0x532793 mov ebx,eax
    pin(out, ContinueBUnit::mov_ebx_eax);

    // 0x532795 mov r12b,bl — bl is the low byte of eax.
    const std::uint8_t r12b = static_cast<std::uint8_t>(eax & 0xFF);
    pin(out, ContinueBUnit::mov_r12b_bl);

    // 0x532798 xor r12b,1
    const std::uint8_t edi_arg = r12b ^ 1;
    pin(out, ContinueBUnit::xor_r12b_1);
    ++out.alu_count;

    // 0x53279c movzx edi,r12b — edi = (bl xor 1) zero-extended.
    pin(out, ContinueBUnit::movzx_edi_r12b);

    // 0x5327a0 call 0x4c4950 (body NOT modelled).
    if (hooks.call_4c4950 != nullptr) {
        hooks.call_4c4950(edi_arg);
    }
    pin(out, ContinueBUnit::call_4c4950);
    ++out.call_count;

    // 0x5327a5 mov rdi,[r13+0] — r13 is the 0x32f84f8 cell set by continue_a;
    // provenance NOT modelled here. Slot supplies the load.
    // rdi is dead in this span (consumed only by a successor block NOT modelled).
    if (hooks.cell_32f84f8 != nullptr) {
        (void)*hooks.cell_32f84f8;  // load observed, value is dead in this span
        ++out.load_count;
    }
    pin(out, ContinueBUnit::mov_rdi_r13_load);

    // 0x5327a9 test bl,bl — flags are NOT evaluated.
    pin(out, ContinueBUnit::test_bl_bl);
    ++out.alu_count;

    // 0x5327ab je 0x5328e0 — structural boundary. Target pinned by the checker;
    // outcome and both successor blocks (0x5327b1 / 0x5328e0) NOT modelled.
    pin(out, ContinueBUnit::je_5328e0);
    ++out.branch_boundary_count;
    out.branch_outcome_modelled = false;  // je outcome / both successors NOT modelled

    out.ok = (out.unit_executed == kInit531d60PostBaseContinueBUnitCount) &&
             (out.call_count == 2) && (out.alu_count == 5) && (out.load_count <= 1) &&
             (out.branch_boundary_count == 1) && (out.unit_counts[0] == 1) &&
             (out.unit_counts[kInit531d60PostBaseContinueBUnitCount - 1] == 1);
    return out;
}

}  // namespace sotc
