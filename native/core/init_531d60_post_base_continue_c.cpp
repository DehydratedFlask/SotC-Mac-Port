#include "init_531d60_post_base_continue_c.hpp"

#include "init_531d60_post_base_a431e0_u1.hpp"
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

void* load_ptr(const void* base, std::size_t offset) noexcept {
    void* value = nullptr;
    if (base == nullptr) {
        return nullptr;
    }
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof(value));
    return value;
}

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

    std::uint8_t r14b_in = hooks.incoming_r14b;
    if (hooks.enter_via_5328e0) {
        const bool native = hooks.use_native_lazy_bind_91210 && hooks.demo_object != nullptr;
        if (!native && (hooks.vcall_slot0 == nullptr || hooks.preamble_flag_byte == nullptr)) {
            return out;
        }
        ++out.preamble_unit_executed;
        ++out.preamble_unit_executed;
        ++out.preamble_unit_executed;
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
        ++out.preamble_unit_executed;
        ++out.preamble_unit_executed;
        ++out.preamble_unit_executed;
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

    if (!hooks.merge_a431e0_u1_ran) {
        Init531d60PostBaseA431e0U1Hooks a43{};
        a43.cell_32f6d18 = hooks.cell_32f6d18;
        a43.cell_2efe318 = hooks.cell_2efe318;
        a43.flag_byte_iter2 = hooks.flag_byte_iter2;
        a43.vcall_slot0 = hooks.vcall_slot0;
        a43.call_a431e0 = hooks.call_a431e0;
        a43.use_native_lazy_bind_91210 = hooks.use_native_lazy_bind_91210;
        a43.demo_object = hooks.demo_object;
        a43.lazy_bind = hooks.lazy_bind;
        const Init531d60PostBaseA431e0U1Result a43r = init_531d60_post_base_a431e0_u1(a43);
        out.lazy_bind_calls += a43r.lazy_bind_calls;
        if (a43r.lazy_bind_calls != 0) {
            out.lazy_bind_ok = a43r.lazy_bind_ok;
        }
        if (!a43r.ok) {
            return out;
        }
        out.flag2_known = a43r.flag2_known;
        out.sil_iter2 = a43r.sil_iter2;
    }

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

    pin(out, ContinueCUnit::je_5329ae);
    ++out.branch_boundary_count;
    out.branch_outcome_modelled = false;
    const std::uint8_t* merge_flag = resolve_flag(hooks, hooks.merge_flag_byte);
    if (merge_flag != nullptr) {
        out.merge_branch_outcome_modelled = true;
        out.merge_branch_taken = (*merge_flag == 0);
    }

    out.ok = (out.unit_executed == kInit531d60PostBaseContinueCUnitCount) &&
             (out.call_count == 0) && (out.vcall_count == 1) && (out.lea_count == 0) &&
             (out.global_mov_count == 2) && (out.xor_count == 1) && (out.cmp_count == 1) &&
             (out.sete_count == 0) && (out.branch_boundary_count == 1) &&
             (out.unit_counts[0] == 1) &&
             (out.unit_counts[kInit531d60PostBaseContinueCUnitCount - 1] == 1);
    return out;
}

}  // namespace sotc
