#include "init_531d60_post_base_continue_c.hpp"

#include "init_531d60_post_base_a52b70_u1.hpp"
#include "init_531d60_post_base_a599e0_u1.hpp"

#include <cstring>

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

void* load_ptr(const void* base, std::size_t offset) noexcept {
    void* value = nullptr;
    if (base == nullptr) {
        return nullptr;
    }
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof(value));
    return value;
}

// rax = *(bound+0x60); the compared byte is *rax.
const std::uint8_t* flag_byte_from_demo(const void* demo_object) noexcept {
    const void* bound = load_ptr(demo_object, kLazyBind91210BoundOffset);
    if (bound == nullptr) {
        return nullptr;
    }
    return static_cast<const std::uint8_t*>(load_ptr(bound, 0x60));
}

bool dispatch_slot0(Init531d60PostBaseContinueCResult& out,
                    const Init531d60PostBaseContinueCHooks& hooks) noexcept {
    if (hooks.use_native_lazy_bind_91210) {
        const LazyBind91210Result bound = run_lazy_bind_91210(hooks.demo_object, hooks.lazy_bind);
        ++out.lazy_bind_calls;
        out.lazy_bind_last_returned = bound.returned;
        out.lazy_bind_ok = bound.ok;
        return bound.ok;
    }
    if (hooks.vcall_slot0 != nullptr) {
        hooks.vcall_slot0(const_cast<void*>(hooks.cell_2efe318 != nullptr
                                                ? static_cast<const void*>(hooks.cell_2efe318)
                                                : nullptr),
                          0);
    }
    return true;
}

const std::uint8_t* resolve_flag(const Init531d60PostBaseContinueCHooks& hooks,
                                 const std::uint8_t* supplied) noexcept {
    if (supplied != nullptr) {
        return supplied;
    }
    if (!hooks.use_native_lazy_bind_91210) {
        return nullptr;
    }
    return flag_byte_from_demo(hooks.demo_object);
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
        const bool native = hooks.use_native_lazy_bind_91210 && hooks.demo_object != nullptr;
        if (!native && (hooks.vcall_slot0 == nullptr || hooks.preamble_flag_byte == nullptr)) {
            return out;  // ok == false, nothing pinned
        }
        // 0x5328e0 movq 0x2efe318(%rip),%rax / 0x5328e7 leaq 0x2efe318,%rdi
        ++out.preamble_unit_executed;
        ++out.preamble_unit_executed;
        // 0x5328ee xorl %esi,%esi
        ++out.preamble_unit_executed;
        // 0x5328f0 callq *(%rax) — slot 0 with rdi = vtable cell address, esi = 0
        out.preamble_call_count = 0;
        if (!dispatch_slot0(out, hooks)) {
            return out;
        }
        ++out.preamble_unit_executed;
        ++out.preamble_call_count;
        const std::uint8_t* preamble_flag = resolve_flag(hooks, hooks.preamble_flag_byte);
        if (preamble_flag == nullptr) {
            return out;
        }
        // 0x5328f2 movq 0x2efe340(%rip),%rax
        ++out.preamble_unit_executed;
        // 0x5328f9 movq 0x60(%rax),%rax
        ++out.preamble_unit_executed;
        // 0x5328fd cmpb $0,(%rax)
        ++out.preamble_unit_executed;
        // 0x532900 sete %r14b — r14b = (*flag_byte == 0)
        out.preamble_r14b = (*preamble_flag == 0) ? 1 : 0;
        out.preamble_flag_known = true;
        ++out.preamble_unit_executed;
        out.preamble_ran = true;
        r14b_in = out.preamble_r14b;
    }

    if (!hooks.merge_a52b70_u1_ran) {
        Init531d60PostBaseA52b70U1Hooks u1{};
        u1.incoming_r14b = r14b_in;
        u1.incoming_r15 = hooks.incoming_r15;
        u1.call_a52b70 = hooks.call_a52b70;
        const Init531d60PostBaseA52b70U1Result u1r = init_531d60_post_base_a52b70_u1(u1);
        if (!u1r.ok) {
            return out;
        }
    }

    out.first_esi = r14b_in;

    if (!hooks.merge_a599e0_u1_ran) {
        Init531d60PostBaseA599e0U1Hooks a59{};
        a59.cell_32f8b80 = hooks.cell_32f8b80;
        a59.cell_2efe318 = hooks.cell_2efe318;
        a59.flag_byte_iter1 = hooks.flag_byte_iter1;
        a59.vcall_slot0 = hooks.vcall_slot0;
        a59.call_a599e0 = hooks.call_a599e0;
        a59.use_native_lazy_bind_91210 = hooks.use_native_lazy_bind_91210;
        a59.demo_object = hooks.demo_object;
        a59.lazy_bind = hooks.lazy_bind;
        const Init531d60PostBaseA599e0U1Result a59r = init_531d60_post_base_a599e0_u1(a59);
        out.lazy_bind_calls += a59r.lazy_bind_calls;
        if (a59r.lazy_bind_calls != 0) {
            out.lazy_bind_ok = a59r.lazy_bind_ok;
        }
        if (!a59r.ok) {
            return out;
        }
        out.flag1_known = a59r.flag1_known;
        out.sil_iter1 = a59r.sil_iter1;
    }

    // 0x53294b lea rax,[0x32f6d18] — obj B cell.
    pin(out, ContinueCUnit::lea_rax_32f6d18);
    ++out.lea_count;

    // ---- iteration 2 (obj B) ----
    pin(out, ContinueCUnit::xor_esi_esi_1);
    ++out.xor_count;
    pin(out, ContinueCUnit::mov_rdi_r15_1);
    const void* rbx_2 = (hooks.cell_32f6d18 != nullptr) ? *hooks.cell_32f6d18 : nullptr;
    pin(out, ContinueCUnit::mov_rbx_rax_1);

    pin(out, ContinueCUnit::mov_rax_2efe318_1);
    ++out.global_mov_count;
    if (!dispatch_slot0(out, hooks)) {
        return out;
    }
    pin(out, ContinueCUnit::vcall_slot0_1);
    ++out.vcall_count;

    pin(out, ContinueCUnit::mov_rax_2efe340_1);
    ++out.global_mov_count;
    pin(out, ContinueCUnit::xor_esi_esi_2);
    ++out.xor_count;
    pin(out, ContinueCUnit::mov_rdi_rbx_1);

    pin(out, ContinueCUnit::mov_rax_rax60_1);
    pin(out, ContinueCUnit::cmp_byte_rax0_1);
    ++out.cmp_count;
    const std::uint8_t* flag2 = resolve_flag(hooks, hooks.flag_byte_iter2);
    const std::uint8_t sil_2 = flag_from(flag2);
    out.flag2_known = (flag2 != nullptr);
    out.sil_iter2 = sil_2;
    pin(out, ContinueCUnit::sete_sil_1);
    ++out.sete_count;

    if (hooks.call_a431e0 != nullptr) {
        hooks.call_a431e0(const_cast<void*>(rbx_2), sil_2);
    }
    pin(out, ContinueCUnit::call_a431e0);
    ++out.call_count;

    // ---- iteration 3 (bare vtable dispatch) ----
    pin(out, ContinueCUnit::mov_rax_2efe318_2);
    ++out.global_mov_count;
    pin(out, ContinueCUnit::xor_esi_esi_3);
    ++out.xor_count;
    pin(out, ContinueCUnit::mov_rdi_r15_2);

    if (!dispatch_slot0(out, hooks)) {
        return out;
    }
    pin(out, ContinueCUnit::vcall_slot0_2);
    ++out.vcall_count;

    pin(out, ContinueCUnit::mov_rax_2efe340_2);
    ++out.global_mov_count;
    pin(out, ContinueCUnit::mov_rax_rax60_2);
    pin(out, ContinueCUnit::cmp_byte_rax0_2);
    ++out.cmp_count;

    // 0x53299b je 0x5329ae — structural boundary. Target pinned by the checker;
    // outcome and both successor blocks (0x53299d fall-through, whose head is
    // `lea rax,[0x32f79a8]`, and 0x5329ae branch target) NOT modelled.
    pin(out, ContinueCUnit::je_5329ae);
    ++out.branch_boundary_count;
    out.branch_outcome_modelled = false;  // default; successors stay unmodelled
    // E530: the merge `je 0x5329ae` tests `*(*(0x2efe340) + 0x60)` against zero; when the
    // resolved byte is supplied the outcome is modelled (taken == byte is 0).
    const std::uint8_t* merge_flag = resolve_flag(hooks, hooks.merge_flag_byte);
    if (merge_flag != nullptr) {
        out.merge_branch_outcome_modelled = true;
        out.merge_branch_taken = (*merge_flag == 0);
    }

    out.ok = (out.unit_executed == kInit531d60PostBaseContinueCUnitCount) &&
             (out.call_count == 1) && (out.vcall_count == 2) && (out.lea_count == 1) &&
             (out.global_mov_count == 4) && (out.xor_count == 3) && (out.cmp_count == 2) &&
             (out.sete_count == 1) && (out.branch_boundary_count == 1) &&
             (out.unit_counts[0] == 1) &&
             (out.unit_counts[kInit531d60PostBaseContinueCUnitCount - 1] == 1);
    return out;
}

}  // namespace sotc
