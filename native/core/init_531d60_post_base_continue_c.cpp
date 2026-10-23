#include "init_531d60_post_base_continue_c.hpp"

namespace sotc {
namespace {

int& slot(Init531d60PostBaseContinueCResult& out, ContinueCUnit u) noexcept {
    return out.unit_counts[static_cast<int>(u)];
}

void pin(Init531d60PostBaseContinueCResult& out, ContinueCUnit u) noexcept {
    ++slot(out, u);
    ++out.unit_executed;
}

// Host stand-in for `cmp byte [rax],0` -> `sete sil`. The real operand is
// [0x2efe340]->[+0x60]->byte; the host supplies the resolved byte location.
// A null location => value unknown, place-holder 0 (still counted).
std::uint8_t flag_from(const std::uint8_t* byte) noexcept {
    if (byte == nullptr) {
        return 0;
    }
    return (*byte == 0) ? 1 : 0;
}

}  // namespace

Init531d60PostBaseContinueCResult init_531d60_post_base_continue_c(
    const Init531d60PostBaseContinueCHooks& hooks) noexcept {
    Init531d60PostBaseContinueCResult out{};

    // E529: taken-path preamble 0x5328e0..0x532904 (8 units, 36 B). It polls the
    // same vtable source and derives `sete r14b` from [0x2efe340]+0x60. Without
    // the resolved flag byte and the slot-0 hook it fails visibly instead of
    // inventing a branch value.
    std::uint8_t r14b_in = hooks.incoming_r14b;
    if (hooks.enter_via_5328e0) {
        if (hooks.vcall_slot0 == nullptr || hooks.preamble_flag_byte == nullptr) {
            return out;  // ok == false, nothing pinned
        }
        // 0x5328e0 movq 0x2efe318(%rip),%rax / 0x5328e7 leaq 0x2efe318,%rdi
        ++out.preamble_unit_executed;
        ++out.preamble_unit_executed;
        // 0x5328ee xorl %esi,%esi
        ++out.preamble_unit_executed;
        // 0x5328f0 callq *(%rax) — slot 0 with rdi = vtable cell address, esi = 0
        out.preamble_call_count = 0;
        hooks.vcall_slot0(
            const_cast<void*>(hooks.cell_2efe318 != nullptr
                                  ? static_cast<const void*>(hooks.cell_2efe318)
                                  : nullptr),
            0);
        ++out.preamble_unit_executed;
        ++out.preamble_call_count;
        // 0x5328f2 movq 0x2efe340(%rip),%rax
        ++out.preamble_unit_executed;
        // 0x5328f9 movq 0x60(%rax),%rax
        ++out.preamble_unit_executed;
        // 0x5328fd cmpb $0,(%rax)
        ++out.preamble_unit_executed;
        // 0x532900 sete %r14b — r14b = (*flag_byte == 0)
        out.preamble_r14b = (*hooks.preamble_flag_byte == 0) ? 1 : 0;
        out.preamble_flag_known = true;
        ++out.preamble_unit_executed;
        out.preamble_ran = true;
        r14b_in = out.preamble_r14b;
    }

    // 0x532904 movzx esi,r14b — r14b is 0 on the fall-through path and the
    // preamble's sete r14b on the 0x5328e0 taken path.
    const std::uint8_t esi_1 = r14b_in;
    out.first_esi = esi_1;
    pin(out, ContinueCUnit::movzx_esi_r14b);

    // 0x532908 mov rdi,r15 — r15 provenance upstream, NOT modelled.
    void* rdi_1 = const_cast<void*>(hooks.incoming_r15);
    pin(out, ContinueCUnit::mov_rdi_r15);

    // 0x53290b call 0xa52b70 (body NOT modelled).
    if (hooks.call_a52b70 != nullptr) {
        hooks.call_a52b70(rdi_1, esi_1);
    }
    pin(out, ContinueCUnit::call_a52b70);
    ++out.call_count;

    // 0x532910 lea rax,[0x32f8b80] — obj A cell; rbx = *cell.
    pin(out, ContinueCUnit::lea_rax_32f8b80);
    ++out.lea_count;

    // 0x532917 lea r15,[0x2efe318] — r15 is reused as the vtable source address.
    pin(out, ContinueCUnit::lea_r15_2efe318);
    ++out.lea_count;

    // ---- iteration 1 (obj A) ----
    // 0x53291e xor esi,esi / 0x532920 mov rdi,r15 / 0x532923 mov rbx,[rax]
    pin(out, ContinueCUnit::xor_esi_esi_1);
    ++out.xor_count;
    pin(out, ContinueCUnit::mov_rdi_r15_1);
    const void* rbx_1 = (hooks.cell_32f8b80 != nullptr) ? *hooks.cell_32f8b80 : nullptr;
    pin(out, ContinueCUnit::mov_rbx_rax_1);

    // 0x532926 mov rax,[0x2efe318] / 0x53292d call [rax] (vtable slot 0).
    pin(out, ContinueCUnit::mov_rax_2efe318_1);
    ++out.global_mov_count;
    if (hooks.vcall_slot0 != nullptr) {
        hooks.vcall_slot0(const_cast<void*>(hooks.cell_2efe318 != nullptr
                                                ? static_cast<const void*>(hooks.cell_2efe318)
                                                : nullptr),
                          0);
    }
    pin(out, ContinueCUnit::vcall_slot0_1);
    ++out.vcall_count;

    // 0x53292f mov rax,[0x2efe340] / 0x532936 xor esi,esi / 0x532938 mov rdi,rbx
    pin(out, ContinueCUnit::mov_rax_2efe340_1);
    ++out.global_mov_count;
    pin(out, ContinueCUnit::xor_esi_esi_2);
    ++out.xor_count;
    pin(out, ContinueCUnit::mov_rdi_rbx_1);

    // 0x53293b mov rax,[rax+0x60] / 0x53293f cmp byte [rax],0 / 0x532942 sete sil
    pin(out, ContinueCUnit::mov_rax_rax60_1);
    pin(out, ContinueCUnit::cmp_byte_rax0_1);
    ++out.cmp_count;
    const std::uint8_t sil_1 = flag_from(hooks.flag_byte_iter1);
    out.flag1_known = (hooks.flag_byte_iter1 != nullptr);
    out.sil_iter1 = sil_1;
    pin(out, ContinueCUnit::sete_sil_1);
    ++out.sete_count;

    // 0x532946 call 0xa599e0 (body NOT modelled).
    if (hooks.call_a599e0 != nullptr) {
        hooks.call_a599e0(const_cast<void*>(rbx_1), sil_1);
    }
    pin(out, ContinueCUnit::call_a599e0);
    ++out.call_count;

    // 0x53294b lea rax,[0x32f6d18] — obj B cell.
    pin(out, ContinueCUnit::lea_rax_32f6d18);
    ++out.lea_count;

    // ---- iteration 2 (obj B) ----
    // 0x532952 xor esi,esi / 0x532954 mov rdi,r15 / 0x532957 mov rbx,[rax]
    pin(out, ContinueCUnit::xor_esi_esi_3);
    ++out.xor_count;
    pin(out, ContinueCUnit::mov_rdi_r15_2);
    const void* rbx_2 = (hooks.cell_32f6d18 != nullptr) ? *hooks.cell_32f6d18 : nullptr;
    pin(out, ContinueCUnit::mov_rbx_rax_2);

    // 0x53295a mov rax,[0x2efe318] / 0x532961 call [rax] (vtable slot 0).
    pin(out, ContinueCUnit::mov_rax_2efe318_2);
    ++out.global_mov_count;
    if (hooks.vcall_slot0 != nullptr) {
        hooks.vcall_slot0(const_cast<void*>(hooks.cell_2efe318 != nullptr
                                                ? static_cast<const void*>(hooks.cell_2efe318)
                                                : nullptr),
                          0);
    }
    pin(out, ContinueCUnit::vcall_slot0_2);
    ++out.vcall_count;

    // 0x532963 mov rax,[0x2efe340] / 0x53296a xor esi,esi / 0x53296c mov rdi,rbx
    pin(out, ContinueCUnit::mov_rax_2efe340_2);
    ++out.global_mov_count;
    pin(out, ContinueCUnit::xor_esi_esi_4);
    ++out.xor_count;
    pin(out, ContinueCUnit::mov_rdi_rbx_2);

    // 0x53296f mov rax,[rax+0x60] / 0x532973 cmp byte [rax],0 / 0x532976 sete sil
    pin(out, ContinueCUnit::mov_rax_rax60_2);
    pin(out, ContinueCUnit::cmp_byte_rax0_2);
    ++out.cmp_count;
    const std::uint8_t sil_2 = flag_from(hooks.flag_byte_iter2);
    out.flag2_known = (hooks.flag_byte_iter2 != nullptr);
    out.sil_iter2 = sil_2;
    pin(out, ContinueCUnit::sete_sil_2);
    ++out.sete_count;

    // 0x53297a call 0xa431e0 (body NOT modelled).
    if (hooks.call_a431e0 != nullptr) {
        hooks.call_a431e0(const_cast<void*>(rbx_2), sil_2);
    }
    pin(out, ContinueCUnit::call_a431e0);
    ++out.call_count;

    // ---- iteration 3 (no lea; reuses the vtable source for a bare dispatch) ----
    // 0x53297f mov rax,[0x2efe318] / 0x532986 xor esi,esi / 0x532988 mov rdi,r15
    pin(out, ContinueCUnit::mov_rax_2efe318_3);
    ++out.global_mov_count;
    pin(out, ContinueCUnit::xor_esi_esi_5);
    ++out.xor_count;
    pin(out, ContinueCUnit::mov_rdi_r15_3);

    // 0x53298b call [rax] (vtable slot 0).
    if (hooks.vcall_slot0 != nullptr) {
        hooks.vcall_slot0(const_cast<void*>(hooks.cell_2efe318 != nullptr
                                                ? static_cast<const void*>(hooks.cell_2efe318)
                                                : nullptr),
                          0);
    }
    pin(out, ContinueCUnit::vcall_slot0_3);
    ++out.vcall_count;

    // 0x53298d mov rax,[0x2efe340] / 0x532994 mov rax,[rax+0x60] /
    // 0x532998 cmp byte [rax],0 — flags NOT evaluated (feed the je, NOT modelled).
    pin(out, ContinueCUnit::mov_rax_2efe340_3);
    ++out.global_mov_count;
    pin(out, ContinueCUnit::mov_rax_rax60_3);
    pin(out, ContinueCUnit::cmp_byte_rax0_3);
    ++out.cmp_count;

    // 0x53299b je 0x5329ae — structural boundary. Target pinned by the checker;
    // outcome and both successor blocks (0x53299d fall-through, whose head is
    // `lea rax,[0x32f79a8]`, and 0x5329ae branch target) NOT modelled.
    pin(out, ContinueCUnit::je_5329ae);
    ++out.branch_boundary_count;
    out.branch_outcome_modelled = false;  // je outcome / both successors NOT modelled
    // E530: the merge `je 0x5329ae` tests `*(*(0x2efe340) + 0x60)` against zero; when the
    // resolved byte is supplied the outcome is modelled (taken == byte is 0).
    if (hooks.merge_flag_byte != nullptr) {
        out.merge_branch_outcome_modelled = true;
        out.merge_branch_taken = (*hooks.merge_flag_byte == 0);
    }

    out.ok = (out.unit_executed == kInit531d60PostBaseContinueCUnitCount) &&
             (out.call_count == 3) && (out.vcall_count == 3) && (out.lea_count == 3) &&
             (out.global_mov_count == 6) && (out.xor_count == 5) && (out.cmp_count == 3) &&
             (out.sete_count == 2) && (out.branch_boundary_count == 1) &&
             (out.unit_counts[0] == 1) &&
             (out.unit_counts[kInit531d60PostBaseContinueCUnitCount - 1] == 1);
    return out;
}

}  // namespace sotc
