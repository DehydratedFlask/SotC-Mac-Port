#include "init_531d60_post_base_continue_a.hpp"

#include "config_bool_lookup_1e4c0.hpp"
#include "lookup_1fc80.hpp"

#include <strings.h>

namespace sotc {
namespace {

int strcasecmp_plt(const char* a, const char* b) noexcept {
    if (a == nullptr || b == nullptr) {
        return (a == b) ? 0 : 1;
    }
    return strcasecmp(a, b);
}

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

    // 0x532775 call 0x1fc80.
    if (hooks.call_1fc80 != nullptr) {
        hooks.call_1fc80(rdi_532775, kInit531d60PostBaseContinueALea1014d69Rva);
    }
    void* lookup_rax = nullptr;
    bool lookup_known = false;
    if (hooks.call_1fc80_result != nullptr) {
        lookup_rax = hooks.call_1fc80_result(rdi_532775, kInit531d60PostBaseContinueALea1014d69Rva);
        lookup_known = true;
    } else if (hooks.use_native_lookup_1fc80 && hooks.nointro_needle != nullptr) {
        Lookup1fc80Hooks lookup_hooks{};
        lookup_hooks.compare_f8f500 = &strcasecmp_plt;
        lookup_rax = lookup_1fc80(rdi_532775, hooks.nointro_needle, lookup_hooks);
        lookup_known = true;
    }
    pin(out, ContinueAUnit::call_1fc80);
    ++out.call_count;

    pin(out, ContinueAUnit::test_rax_rax);
    ++out.alu_count;

    pin(out, ContinueAUnit::je_5327bd);
    ++out.branch_boundary_count;
    out.branch_outcome_modelled = false;
    if (lookup_known) {
        out.rax_from_1fc80 = lookup_rax;
        out.rax_from_1fc80_known = true;
        out.branch_outcome_modelled = true;
        out.branch_taken = (lookup_rax == nullptr);
    }

    out.ok = (out.unit_executed == kInit531d60PostBaseContinueAUnitCount) &&
             (out.call_count == 1) && (out.lea_count == 2) && (out.alu_count == 1) &&
             (out.branch_boundary_count == 1) && (out.unit_counts[0] == 1) &&
             (out.unit_counts[kInit531d60PostBaseContinueAUnitCount - 1] == 1);
    return out;
}

InitContinueAbNativeResult run_init_continue_ab_native_lookups(
    Init531d60PostBaseContinueAHooks a_hooks,
    Init531d60PostBaseContinueBHooks b_hooks) noexcept {
    InitContinueAbNativeResult out{};
    a_hooks.use_native_lookup_1fc80 = true;
    out.cont_a = init_531d60_post_base_continue_a(a_hooks);
    if (!out.cont_a.rax_from_1fc80_known || out.cont_a.branch_taken ||
        out.cont_a.rax_from_1fc80 == nullptr) {
        return out;
    }
    b_hooks.incoming_rax_standin = out.cont_a.rax_from_1fc80;
    if (b_hooks.call_1e4c0 == nullptr) {
        b_hooks.call_1e4c0 = &config_bool_lookup_init_continue_b;
    }
    out.cont_b = init_531d60_post_base_continue_b(b_hooks);
    out.ran_continue_b = true;
    return out;
}

}  // namespace sotc
