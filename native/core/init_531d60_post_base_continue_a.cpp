#include "init_531d60_post_base_continue_a.hpp"

#include <cstring>

namespace sotc {
namespace {

int& slot(Init531d60PostBaseContinueAResult& out, ContinueAUnit u) noexcept {
    return out.unit_counts[static_cast<int>(u)];
}

void pin(Init531d60PostBaseContinueAResult& out, ContinueAUnit u) noexcept {
    ++slot(out, u);
    ++out.unit_executed;
}

}  // namespace

Init531d60PostBaseContinueAResult init_531d60_post_base_continue_a(
    const Init531d60PostBaseContinueAHooks& hooks) noexcept {
    Init531d60PostBaseContinueAResult out{};

    // 0x532720 mov rdi,r15 — r15 provenance NOT modelled (control-flow pin only).
    pin(out, ContinueAUnit::mov_rdi_r15);

    // 0x532723 call 0xf590 (body NOT modelled).
    if (hooks.call_f590 != nullptr) {
        hooks.call_f590();
    }
    pin(out, ContinueAUnit::call_f590);
    ++out.call_count;

    // 0x532728 mov rbx,[r13] — incoming r13 NOT modelled; slot supplies the load.
    const void* rbx = nullptr;
    if (hooks.incoming_r13_slot != nullptr) {
        rbx = *hooks.incoming_r13_slot;
        ++out.load_count;
    }
    pin(out, ContinueAUnit::mov_rbx_load_r13);

    // 0x53272c xor esi,esi
    pin(out, ContinueAUnit::xor_esi_esi);

    // 0x53272e mov rdi,rbx
    pin(out, ContinueAUnit::mov_rdi_rbx_1);

    // 0x532731 call 0xa2ea00 (body NOT modelled; eax is a host stand-in).
    int eax = 0;
    if (hooks.call_a2ea00 != nullptr) {
        eax = hooks.call_a2ea00(const_cast<void*>(rbx));
    }
    pin(out, ContinueAUnit::call_a2ea00);
    ++out.call_count;

    // 0x532736 mov rdi,rbx
    pin(out, ContinueAUnit::mov_rdi_rbx_2);

    // 0x532739 mov esi,eax — eax stand-in (0 when the a2ea00 hook is null).
    pin(out, ContinueAUnit::mov_esi_eax);

    // 0x53273b call 0xa2e5d0 (body NOT modelled).
    if (hooks.call_a2e5d0 != nullptr) {
        hooks.call_a2e5d0(const_cast<void*>(rbx), eax);
    }
    pin(out, ContinueAUnit::call_a2e5d0);
    ++out.call_count;

    // 0x532740 lea rax,[0x32f79a8] — cell-address pin (optional host stand-in).
    const void* const* cell_32f79a8 = hooks.cell_32f79a8;
    pin(out, ContinueAUnit::lea_rax_32f79a8);
    ++out.lea_count;

    // 0x532747 lea r13,[0x32f84f8] — cell-address pin.
    const void* const* cell_32f84f8 = hooks.cell_32f84f8;
    pin(out, ContinueAUnit::lea_r13_32f84f8);
    ++out.lea_count;

    // 0x53274e mov rax,[rax]
    const void* obj = nullptr;
    if (cell_32f79a8 != nullptr) {
        obj = *cell_32f79a8;
        ++out.load_count;
    }
    pin(out, ContinueAUnit::load_rax_rax);
    // 0x532751 mov rdi,[r13]
    const void* rdi_532760 = nullptr;
    if (cell_32f84f8 != nullptr) {
        rdi_532760 = *cell_32f84f8;
        ++out.load_count;
    }
    pin(out, ContinueAUnit::load_rdi_r13);

    // 0x532755 mov rsi,[rax+0xf8] — 8 B read through the cell's object stand-in.
    std::uintptr_t rsi = 0;
    bool rsi_known = false;
    if (obj != nullptr) {
        const auto* bytes = static_cast<const std::uint8_t*>(obj);
        std::memcpy(&rsi, bytes + kInit531d60PostBaseContinueAObjOff, sizeof(rsi));
        rsi_known = true;
        ++out.load_count;
    }
    pin(out, ContinueAUnit::load_rsi_rax_f8);

    // 0x53275c add rsi,0x40
    if (rsi_known) {
        rsi += kInit531d60PostBaseContinueARsiAddend;
    }
    pin(out, ContinueAUnit::add_rsi_40);
    ++out.alu_count;

    // 0x532760 call 0xa543b0 (body NOT modelled).
    if (hooks.call_a543b0 != nullptr) {
        hooks.call_a543b0(const_cast<void*>(rdi_532760), rsi);
    }
    pin(out, ContinueAUnit::call_a543b0);
    ++out.call_count;
    out.rsi_at_0x532760 = rsi;
    out.rsi_known = rsi_known;

    // 0x532765 mov rax,[rsp+0x20] — only this stack slot is exposed.
    const void* rax = nullptr;
    if (hooks.rsp20_slot != nullptr) {
        rax = *hooks.rsp20_slot;
        ++out.load_count;
    }
    pin(out, ContinueAUnit::load_rax_rsp20);

    // 0x53276a lea rsi,[0x1014d69] — pinned data RVA, not a host pointer.
    pin(out, ContinueAUnit::lea_rsi_1014d69);
    ++out.lea_count;

    // 0x532771 lea rdi,[rax+8]
    auto* rdi_532775 = const_cast<std::uint8_t*>(static_cast<const std::uint8_t*>(rax));
    if (rdi_532775 != nullptr) {
        rdi_532775 += 8;
    }
    pin(out, ContinueAUnit::lea_rdi_rax8);
    ++out.lea_count;

    // 0x532775 call 0x1fc80 (body NOT modelled).
    if (hooks.call_1fc80 != nullptr) {
        hooks.call_1fc80(rdi_532775, kInit531d60PostBaseContinueALea1014d69Rva);
    }
    pin(out, ContinueAUnit::call_1fc80);
    ++out.call_count;

    // 0x53277a test rax,rax — the tested value is the (unmodelled) call result,
    // so only the instruction itself is pinned; flags are NOT evaluated.
    pin(out, ContinueAUnit::test_rax_rax);
    ++out.alu_count;

    // 0x53277d je 0x5327bd — structural boundary. Target pinned by the checker;
    // outcome and both successor blocks (0x53277f / 0x5327bd) NOT modelled.
    pin(out, ContinueAUnit::je_5327bd);
    ++out.branch_boundary_count;
    out.branch_outcome_modelled = false;  // je outcome / both successors NOT modelled

    out.ok = (out.unit_executed == kInit531d60PostBaseContinueAUnitCount) &&
             (out.call_count == 5) && (out.lea_count == 4) && (out.alu_count == 2) &&
             (out.branch_boundary_count == 1) && (out.unit_counts[0] == 1) &&
             (out.unit_counts[kInit531d60PostBaseContinueAUnitCount - 1] == 1);
    return out;
}

}  // namespace sotc
