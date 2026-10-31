#include "init_531d60_post_base_continue_a.hpp"

#include "config_bool_lookup_1e4c0.hpp"
#include "init_531d60_post_base_1fc80_u1.hpp"
#include "init_531d60_post_base_1e4c0_u1.hpp"
#include "init_531d60_post_base_4c4950_u1.hpp"

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

    void* lookup_rax = hooks.incoming_rax_from_1fc80;
    bool lookup_known = hooks.incoming_rax_from_1fc80_known;

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
             (out.call_count == 0) && (out.lea_count == 0) && (out.alu_count == 1) &&
             (out.branch_boundary_count == 1) && (out.unit_counts[0] == 1) &&
             (out.unit_counts[kInit531d60PostBaseContinueAUnitCount - 1] == 1);
    return out;
}

InitContinueAbNativeResult run_init_continue_ab_native_lookups(
    Init531d60PostBaseContinueAHooks a_hooks,
    Init531d60PostBaseContinueBHooks b_hooks) noexcept {
    InitContinueAbNativeResult out{};
    Init531d60PostBase1fc80U1Hooks u1_hooks{};
    u1_hooks.rsp20_slot = a_hooks.rsp20_slot;
    u1_hooks.call_1fc80 = a_hooks.call_1fc80;
    u1_hooks.call_1fc80_result = a_hooks.call_1fc80_result;
    u1_hooks.use_native_lookup_1fc80 = true;
    u1_hooks.nointro_needle = a_hooks.nointro_needle;
    const Init531d60PostBase1fc80U1Result u1 = init_531d60_post_base_1fc80_u1(u1_hooks);
    if (!u1.ok) {
        return out;
    }
    Init531d60PostBaseContinueAHooks ca{};
    ca.incoming_rax_from_1fc80 = u1.rax_from_1fc80;
    ca.incoming_rax_from_1fc80_known = u1.rax_from_1fc80_known;
    out.cont_a = init_531d60_post_base_continue_a(ca);
    if (!out.cont_a.rax_from_1fc80_known || out.cont_a.branch_taken ||
        out.cont_a.rax_from_1fc80 == nullptr) {
        return out;
    }
    if (b_hooks.call_1e4c0 == nullptr) {
        b_hooks.call_1e4c0 = &config_bool_lookup_init_continue_b;
    }
    Init531d60PostBase1e4c0U1Hooks u1e4{};
    u1e4.incoming_rax_standin = out.cont_a.rax_from_1fc80;
    u1e4.call_1e4c0 = b_hooks.call_1e4c0;
    const Init531d60PostBase1e4c0U1Result e4 = init_531d60_post_base_1e4c0_u1(u1e4);
    if (!e4.ok) {
        return out;
    }
    Init531d60PostBase4c4950U1Hooks u4{};
    u4.incoming_eax_from_1e4c0_u1 = e4.eax_return;
    u4.incoming_eax_known = e4.eax_known;
    u4.call_4c4950 = b_hooks.call_4c4950;
    const Init531d60PostBase4c4950U1Result c49 = init_531d60_post_base_4c4950_u1(u4);
    if (!c49.ok) {
        return out;
    }
    Init531d60PostBaseContinueBHooks cb = b_hooks;
    cb.incoming_eax_from_1e4c0_u1 = e4.eax_return;
    cb.incoming_eax_known = e4.eax_known;
    out.cont_b = init_531d60_post_base_continue_b(cb);
    out.ran_continue_b = true;
    return out;
}

}  // namespace sotc
