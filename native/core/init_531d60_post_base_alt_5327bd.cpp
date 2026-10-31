#include "init_531d60_post_base_alt_5327bd.hpp"

#include <cstring>

namespace sotc {
namespace {

int& slot(Init531d60PostBaseAlt5327bdResult& out, Alt5327bdUnit u) noexcept {
    return out.unit_counts[static_cast<int>(u)];
}

void pin(Init531d60PostBaseAlt5327bdResult& out, Alt5327bdUnit u) noexcept {
    ++slot(out, u);
    ++out.unit_executed;
}

void* load_ptr(const void* base, std::size_t offset) noexcept {
    void* value = nullptr;
    if (base != nullptr) {
        std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset,
                    sizeof(value));
    }
    return value;
}

const std::uint8_t* flag_byte_from_demo(const void* demo_object) noexcept {
    const void* bound = load_ptr(demo_object, kLazyBind91210BoundOffset);
    if (bound == nullptr) {
        return nullptr;
    }
    return static_cast<const std::uint8_t*>(load_ptr(bound, 0x60));
}

}  // namespace

Init531d60PostBaseAlt5327bdResult init_531d60_post_base_alt_5327bd(
    const Init531d60PostBaseAlt5327bdHooks& hooks) noexcept {
    Init531d60PostBaseAlt5327bdResult out{};

    // All 70 units pinned unconditionally.

    // === Gate check (units 0–3) ===
    pin(out, Alt5327bdUnit::lea_rax_2dfce70);       ++out.lea_rip_count;
    pin(out, Alt5327bdUnit::mov_rax_rax60);
    pin(out, Alt5327bdUnit::cmp_byte_rax_0);

    bool gate_open = true;
    bool gate_known = false;
    if (hooks.gate_check_supplied) {
        gate_known = true;
        gate_open = (hooks.gate_check_result == 0);
    } else if (hooks.gate_2dfce70 != nullptr) {
        gate_known = true;
        gate_open = (static_cast<const std::uint8_t*>(
            hooks.gate_2dfce70)[kAlt5327bdGateOff] == 0);
    }
    pin(out, Alt5327bdUnit::jne_5328cf);             ++out.branch_count;

    // === Object cell + vtable dispatch (units 4–28) ===
    pin(out, Alt5327bdUnit::lea_rbx_3305568);        ++out.lea_rip_count;
    pin(out, Alt5327bdUnit::mov_rax_3f80);
    pin(out, Alt5327bdUnit::lea_rdx_rsp28);
    pin(out, Alt5327bdUnit::mov_esi_7);
    pin(out, Alt5327bdUnit::mov_rsp28_rax);
    pin(out, Alt5327bdUnit::mov_dword_rsp30_2);
    pin(out, Alt5327bdUnit::mov_rdi_rbx_1);

    void* obj = const_cast<void*>(hooks.obj_3305568);
    if (gate_known && gate_open && hooks.call_ac5260 != nullptr)
        hooks.call_ac5260(obj, nullptr, 7);
    pin(out, Alt5327bdUnit::call_ac5260);            ++out.call_direct_count;

    pin(out, Alt5327bdUnit::mov_rdi_rbx_2);
    pin(out, Alt5327bdUnit::mov_rax_rdi_1);
    pin(out, Alt5327bdUnit::vcall_rax48);            ++out.vcall_count;

    pin(out, Alt5327bdUnit::mov_rdi_rbx_3);
    pin(out, Alt5327bdUnit::xor_esi_esi_1);
    pin(out, Alt5327bdUnit::mov_rax_rdi_2);
    pin(out, Alt5327bdUnit::vcall_rax28);            ++out.vcall_count;

    pin(out, Alt5327bdUnit::mov_rdi_rbx_4);
    pin(out, Alt5327bdUnit::mov_rax_rdi_3);
    pin(out, Alt5327bdUnit::vcall_rax50);            ++out.vcall_count;

    pin(out, Alt5327bdUnit::mov_rsi_rbx);
    pin(out, Alt5327bdUnit::lea_rdi_rsp570);
    pin(out, Alt5327bdUnit::xor_edx_edx);
    pin(out, Alt5327bdUnit::mov_rax_rsi);
    pin(out, Alt5327bdUnit::vcall_rax20);            ++out.vcall_count;

    // === Float compare (units 29–31) ===
    pin(out, Alt5327bdUnit::vmovss_xmm0_rsp58c);
    pin(out, Alt5327bdUnit::vucomiss_xmm0_1014c40);  ++out.float_compare_count;

    bool float_ok = true;
    if (hooks.float_compare_supplied) float_ok = !hooks.float_below_equal;

    // A known failed predicate is sufficient to prove the error route. Success
    // requires both branches to be known and not taken; default stand-ins are
    // not evidence of either route.
    const bool error_path_known = (gate_known && !gate_open) ||
                                  (hooks.float_compare_supplied && !float_ok);
    const bool success_path_known = gate_known && gate_open &&
                                    hooks.float_compare_supplied && float_ok;

    pin(out, Alt5327bdUnit::jbe_5328cf);             ++out.branch_count;

    // === Container check (units 32–38) ===
    pin(out, Alt5327bdUnit::lea_rcx_2ea5170);        ++out.lea_rip_count;
    pin(out, Alt5327bdUnit::mov_rax_rcx);
    pin(out, Alt5327bdUnit::cmp_rax_rcx8);

    bool list_empty = true;
    if (hooks.container_2ea5170 != nullptr) {
        const auto* c = static_cast<const std::uint8_t*>(hooks.container_2ea5170);
        void *h = nullptr, *t = nullptr;
        std::memcpy(&h, c, sizeof(void*));
        std::memcpy(&t, c + 8, sizeof(void*));
        list_empty = (h == t);
    }
    pin(out, Alt5327bdUnit::je_532864);              ++out.branch_count;

    pin(out, Alt5327bdUnit::mov_rax_rax0128);
    pin(out, Alt5327bdUnit::cmp_byte_rax_0_2);

    bool cflag_zero = true;
    if (hooks.container_flag_supplied) cflag_zero = (hooks.container_flag == 0);

    pin(out, Alt5327bdUnit::je_5328bc);              ++out.branch_count;

    // === Logging sub-block (units 39–54) ===
    pin(out, Alt5327bdUnit::lea_rax_1014db9);        ++out.lea_rip_count;
    pin(out, Alt5327bdUnit::lea_r9_1014ded);         ++out.lea_rip_count;
    pin(out, Alt5327bdUnit::lea_rdx_rsp450);
    pin(out, Alt5327bdUnit::mov_edi_0);
    pin(out, Alt5327bdUnit::mov_esi_1);
    pin(out, Alt5327bdUnit::xor_ecx_ecx);
    pin(out, Alt5327bdUnit::mov_r8d_1);
    pin(out, Alt5327bdUnit::vmovq_xmm0_rax);
    pin(out, Alt5327bdUnit::lea_rax_1014d71);        ++out.lea_rip_count;
    pin(out, Alt5327bdUnit::vmovq_xmm1_rax);
    pin(out, Alt5327bdUnit::xor_eax_eax_1);
    pin(out, Alt5327bdUnit::vpunpcklqdq);
    pin(out, Alt5327bdUnit::vmovdqa_rsp450);
    pin(out, Alt5327bdUnit::mov_dword_rsp460_202);

    bool do_log = (!list_empty && cflag_zero) || list_empty;
    if (hooks.skip_logging_supplied) do_log = !hooks.skip_logging;
    if (success_path_known && do_log && hooks.call_3e7e0 != nullptr)
        hooks.call_3e7e0(0, 1, 0, 1, nullptr, nullptr, nullptr);
    pin(out, Alt5327bdUnit::call_3e7e0);             ++out.call_direct_count;

    // === Success cleanup (units 55–59) ===
    pin(out, Alt5327bdUnit::xor_edi_edi);
    pin(out, Alt5327bdUnit::xor_r12d_r12d);
    if (success_path_known && hooks.call_4c4950 != nullptr)
        hooks.call_4c4950(0);
    pin(out, Alt5327bdUnit::call_4c4950_1);          ++out.call_direct_count;
    pin(out, Alt5327bdUnit::mov_r15_r13);
    pin(out, Alt5327bdUnit::xor_r14d_r14d);
    pin(out, Alt5327bdUnit::jmp_532904);             ++out.branch_count;

    // === Error-catch path (units 60–63) ===
    pin(out, Alt5327bdUnit::mov_edi_1);
    if (error_path_known && hooks.call_4c4950 != nullptr)
        hooks.call_4c4950(1);
    pin(out, Alt5327bdUnit::call_4c4950_2);          ++out.call_direct_count;
    pin(out, Alt5327bdUnit::mov_r15_r13_err);
    pin(out, Alt5327bdUnit::mov_r12b_1);

    // === 36 B preamble (units 64–69) ===
    pin(out, Alt5327bdUnit::mov_rax_2efe318);        ++out.mov_rip_count;
    pin(out, Alt5327bdUnit::lea_rdi_2efe318);        ++out.lea_rip_count;
    pin(out, Alt5327bdUnit::xor_esi_esi_2);
    pin(out, Alt5327bdUnit::vcall_rax);              ++out.vcall_count;
    pin(out, Alt5327bdUnit::mov_rax_2efe340);        ++out.mov_rip_count;
    pin(out, Alt5327bdUnit::mov_rax_rax60_2);
    pin(out, Alt5327bdUnit::cmp_byte_rax_0_3);

    const bool error_path = error_path_known;
    const std::uint8_t* flag_byte = nullptr;
    if (error_path) {
        if (hooks.use_native_lazy_bind_91210) {
            ++out.slot0_dispatch_count;
            const LazyBind91210Result bind =
                run_lazy_bind_91210(hooks.demo_object, hooks.lazy_bind);
            ++out.lazy_bind_calls;
            out.lazy_bind_last_returned = bind.returned;
            out.lazy_bind_ok = bind.ok;
            if (!bind.ok) {
                return out;
            }
            flag_byte = flag_byte_from_demo(hooks.demo_object);
        } else {
            if (hooks.vcall_slot0 != nullptr) {
                ++out.slot0_dispatch_count;
                hooks.vcall_slot0(const_cast<void*>(hooks.cell_2efe318), 0);
            }
            flag_byte = (hooks.cell_2efe340 != nullptr)
                            ? static_cast<const std::uint8_t*>(hooks.cell_2efe340) + 0x60
                            : nullptr;
        }
    }
    const bool flag340_zero = (flag_byte != nullptr) && (*flag_byte == 0);
    pin(out, Alt5327bdUnit::sete_r14b);              ++out.setcc_count;

    // === Join flags ===
    if (success_path_known) {
        out.reached_via_jmp = true;
        out.r12b_out = 0;
        out.r14b_out = 0;
    } else if (error_path_known) {
        out.reached_via_preamble = true;
        out.r12b_out = 1;
        out.r14b_out = flag340_zero ? 1 : 0;
    }
    out.r12b_known = error_path_known || success_path_known;
    out.r14b_known = success_path_known || (error_path_known && flag_byte != nullptr);

    out.ok = (out.unit_executed == kInit531d60PostBaseAlt5327bdUnitCount) &&
             (out.call_direct_count == 4) && (out.vcall_count == 5) &&
             (out.lea_rip_count == 7) && (out.mov_rip_count == 2) &&
             (out.float_compare_count == 1) && (out.branch_count == 5) &&
             (out.setcc_count == 1) &&
             (out.unit_counts[0] == 1) &&
             (out.unit_counts[kInit531d60PostBaseAlt5327bdUnitCount - 1] == 1);
    return out;
}

}  // namespace sotc
