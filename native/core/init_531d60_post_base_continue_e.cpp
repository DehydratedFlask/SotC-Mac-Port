#include "init_531d60_post_base_continue_e.hpp"

#include "init_531d60_post_base_a54920_532a62_u1.hpp"
#include "init_531d60_post_base_a54920_u1.hpp"
#include "init_531d60_post_base_vcall18_532a17.hpp"
#include "init_531d60_post_base_vcall18_532a72_u1.hpp"
#include "init_531d60_post_base_lea_5388e0_532a7d_u1.hpp"
#include "init_531d60_post_base_lea_rsp470_532a84_u1.hpp"
#include "init_531d60_post_base_lea_rsi_15c8e18_532a8c_u1.hpp"
#include "init_531d60_post_base_movq_rsp4a0_0_532a93_u1.hpp"
#include "init_531d60_post_base_mov_rdi_r13_532a9f_u1.hpp"
#include "init_531d60_post_base_lea_538870_532aa3_u1.hpp"
#include "init_531d60_post_base_lea_rbx_rsp490_532aaa_u1.hpp"
#include "init_531d60_post_base_mov_rsp470_rdx_532ab2_u1.hpp"
#include "init_531d60_post_base_mov_rsp488_rax_532ac3_u1.hpp"
#include "init_531d60_post_base_mov_rsp4b0_rsi_532acb_u1.hpp"
#include "init_531d60_post_base_vmovdqa_rsp478_xmm0_532aba_u1.hpp"
#include "init_531d60_post_base_vmovq_532a75_u1.hpp"

#include <cstring>

namespace sotc {
namespace {

int& slot(Init531d60PostBaseContinueEResult& out, ContinueEUnit u) noexcept {
    return out.unit_counts[static_cast<int>(u)];
}

void pin(Init531d60PostBaseContinueEResult& out, ContinueEUnit u) noexcept {
    ++slot(out, u);
    ++out.unit_executed;
}

// 0x50-byte stack descriptor used by the three save-data ctor calls.
// The original zeroes it with vmovaps and stores a vtable pointer at +0x40
// and a self-pointer at +0x20.  The native lift mirrors the field offsets but
// populates only the fields consumed by the original code path.
struct SaveDataDescriptor {
    std::uint8_t zero[0x20]{};   // [+0x00..+0x1F] vmovaps-zeroed
    void* self_ptr = nullptr;    // [+0x20..+0x27] self-pointer (= this)
    std::uint64_t gap_28 = 0;    // [+0x28..+0x2F] untouched
    std::uint64_t field_30 = 0;  // [+0x30..+0x37] zeroed
    std::uint64_t gap_38 = 0;    // [+0x38..+0x3F] untouched
    void* vtable = nullptr;      // [+0x40..+0x47] vtable pointer
    std::uint64_t field_48 = 0;  // [+0x48..+0x4F] zeroed
};
static_assert(sizeof(SaveDataDescriptor) == 0x50,
              "descriptor size must match 0x50 B");

}  // namespace

Init531d60PostBaseContinueEResult init_531d60_post_base_continue_e(
    const Init531d60PostBaseContinueEHooks& hooks) noexcept {
    Init531d60PostBaseContinueEResult out{};

    if (!hooks.merge_a54920_u1_ran) {
        Init531d60PostBaseA54920U1Hooks u1{};
        u1.incoming_r13 = hooks.incoming_r13;
        u1.incoming_r12b = hooks.incoming_r12b;
        u1.boss_flag_2dcdf14 = hooks.boss_flag_2dcdf14;
        u1.call_a54920 = hooks.call_a54920;
        const Init531d60PostBaseA54920U1Result u1r = init_531d60_post_base_a54920_u1(u1);
        if (!u1r.ok) {
            return out;
        }
        out.call_a54920_count += u1r.call_a54920_count;
        out.stamp_count += u1r.stamp_count;
        out.lea_rip_count += u1r.lea_rip_count;
        out.boss_flag_before = u1r.boss_flag_before;
        out.boss_flag_after = u1r.boss_flag_after;
    }

    if (!hooks.merge_vcall18_532a17_ran) {
        Init531d60PostBaseVcall18532a17Hooks vc{};
        vc.call_vslot18 = hooks.call_vslot18_532a17;
        const Init531d60PostBaseVcall18532a17Result vcr =
            init_531d60_post_base_vcall18_532a17(vc);
        if (!vcr.ok) {
            return out;
        }
        out.vcall_noop_count += vcr.call_vslot18_count;
    }

    if (!hooks.merge_a54920_532a62_u1_ran) {
        Init531d60PostBaseA54920_532a62_U1Hooks u2{};
        u2.incoming_r13 = hooks.incoming_r13;
        u2.call_a54920 = hooks.call_a54920;
        const Init531d60PostBaseA54920_532a62_U1Result u2r =
            init_531d60_post_base_a54920_532a62_u1(u2);
        if (!u2r.ok) {
            return out;
        }
        out.call_a54920_count += u2r.call_a54920_count;
        out.lea_rip_count += u2r.lea_rip_count;
    }

    if (!hooks.merge_vcall18_532a72_u1_ran) {
        Init531d60PostBaseVcall18_532a72_U1Hooks vc2{};
        vc2.call_vslot18 = hooks.call_vslot18_532a72;
        const Init531d60PostBaseVcall18_532a72_U1Result vcr2 =
            init_531d60_post_base_vcall18_532a72_u1(vc2);
        if (!vcr2.ok) {
            return out;
        }
        out.vcall_noop_count += vcr2.call_vslot18_count;
    }

    // ---- Block 3: save-data ctor 3 (name 0x2efda50, vtable 0x15c8e18) ----
    alignas(16) std::uint8_t xmm0_buf[16]{};
    Init531d60PostBaseVmovq532a75_U1Cells vq_cells = hooks.vmovq_532a75_cells;
    if (!hooks.merge_vmovq_532a75_u1_ran) {
        const Init531d60PostBaseVmovq532a75_U1Result vqr =
            init_531d60_post_base_vmovq_532a75_u1(vq_cells);
        if (!vqr.ok) {
            return out;
        }
        out.vmovq_count += vqr.vmovq_count;
    }
    if (vq_cells.dst_xmm0 != nullptr) {
        std::memcpy(xmm0_buf, vq_cells.dst_xmm0, kInit531d60PostBaseVmovq532a75_U1LaneBytes);
    }

    void* rax_thunk1 = reinterpret_cast<void*>(
        static_cast<std::uintptr_t>(kInit531d60PostBaseContinueEThunk1Rva));
    Init531d60PostBaseLea5388e0_532a7d_U1Cells lea_cells = hooks.lea_5388e0_532a7d_u1_cells;
    if (lea_cells.rax_out == nullptr) {
        lea_cells.rax_out = &rax_thunk1;
    }
    if (!hooks.merge_lea_5388e0_532a7d_u1_ran) {
        const Init531d60PostBaseLea5388e0_532a7d_U1Result lr =
            init_531d60_post_base_lea_5388e0_532a7d_u1(lea_cells);
        if (!lr.ok) {
            return out;
        }
        out.lea_rip_count += lr.lea_rip_count;
    }

    Init531d60PostBaseLeaRsp470_532a84_U1Cells lea_rsp_cells = hooks.lea_rsp470_532a84_u1_cells;
    if (!hooks.merge_lea_rsp470_532a84_u1_ran) {
        const Init531d60PostBaseLeaRsp470_532a84_U1Result lrr =
            init_531d60_post_base_lea_rsp470_532a84_u1(lea_rsp_cells);
        if (!lrr.ok) {
            return out;
        }
    }

    Init531d60PostBaseLeaRsi15c8e18_532a8c_U1Cells lea_rsi_cells =
        hooks.lea_rsi_15c8e18_532a8c_u1_cells;
    if (!hooks.merge_lea_rsi_15c8e18_532a8c_u1_ran) {
        const Init531d60PostBaseLeaRsi15c8e18_532a8c_U1Result lsr =
            init_531d60_post_base_lea_rsi_15c8e18_532a8c_u1(lea_rsi_cells);
        if (!lsr.ok) {
            return out;
        }
        out.lea_rip_count += lsr.lea_rip_count;
    }

    // Descriptor 3 on stack (address pinned by lea rdx,[rsp+0x470] @ 0x532a84 peel).
    alignas(32) SaveDataDescriptor desc3{};
    void* desc3_ptr = &desc3;

    Init531d60PostBaseMovqRsp4a0_0_532a93_U1Cells movq_cells =
        hooks.movq_rsp4a0_0_532a93_u1_cells;
    if (movq_cells.field_30_out == nullptr) {
        movq_cells.field_30_out = &desc3.field_30;
    }
    if (!hooks.merge_movq_rsp4a0_0_532a93_u1_ran) {
        const Init531d60PostBaseMovqRsp4a0_0_532a93_U1Result mqr =
            init_531d60_post_base_movq_rsp4a0_0_532a93_u1(movq_cells);
        if (!mqr.ok) {
            return out;
        }
    } else {
        desc3.field_30 = 0;
    }

    Init531d60PostBaseMovRdiR13_532a9f_U1Cells mov_rdi_cells =
        hooks.mov_rdi_r13_532a9f_u1_cells;
    if (mov_rdi_cells.incoming_r13 == nullptr) {
        mov_rdi_cells.incoming_r13 = hooks.incoming_r13;
    }
    void* parent3 = nullptr;
    if (!hooks.merge_mov_rdi_r13_532a9f_u1_ran) {
        const Init531d60PostBaseMovRdiR13_532a9f_U1Result mrr =
            init_531d60_post_base_mov_rdi_r13_532a9f_u1(mov_rdi_cells);
        if (!mrr.ok) {
            return out;
        }
        parent3 = mrr.parent;
    } else {
        parent3 = const_cast<void*>(hooks.incoming_r13);
    }

    Init531d60PostBaseLea538870_532aa3_U1Cells lea_rcx_cells =
        hooks.lea_538870_532aa3_u1_cells;
    if (!hooks.merge_lea_538870_532aa3_u1_ran) {
        const Init531d60PostBaseLea538870_532aa3_U1Result lcr =
            init_531d60_post_base_lea_538870_532aa3_u1(lea_rcx_cells);
        if (!lcr.ok) {
            return out;
        }
        out.lea_rip_count += lcr.lea_rip_count;
    }

    Init531d60PostBaseLeaRbxRsp490_532aaa_U1Cells lea_rbx_cells =
        hooks.lea_rbx_rsp490_532aaa_u1_cells;
    const void* synthetic_rsp =
        reinterpret_cast<const std::uint8_t*>(desc3_ptr) -
        static_cast<std::uintptr_t>(kInit531d60PostBaseLeaRsp470_532a84_U1Disp);
    if (lea_rbx_cells.rbx_out == nullptr) {
        lea_rbx_cells.rbx_out = &desc3.self_ptr;
    }
    if (lea_rbx_cells.stack_rsp == nullptr) {
        lea_rbx_cells.stack_rsp = synthetic_rsp;
    }
    if (!hooks.merge_lea_rbx_rsp490_532aaa_u1_ran) {
        const Init531d60PostBaseLeaRbxRsp490_532aaa_U1Result lbr =
            init_531d60_post_base_lea_rbx_rsp490_532aaa_u1(lea_rbx_cells);
        if (!lbr.ok) {
            return out;
        }
    } else {
        desc3.self_ptr = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&desc3) + 0x20);
    }

    Init531d60PostBaseMovRsp470Rdx_532ab2_U1Cells mov_head_cells =
        hooks.mov_rsp470_rdx_532ab2_u1_cells;
    if (mov_head_cells.head_out == nullptr) {
        mov_head_cells.head_out = reinterpret_cast<void**>(&desc3.zero[0]);
    }
    if (mov_head_cells.rdx_in == nullptr) {
        mov_head_cells.rdx_in = desc3_ptr;
    }
    if (!hooks.merge_mov_rsp470_rdx_532ab2_u1_ran) {
        const Init531d60PostBaseMovRsp470Rdx_532ab2_U1Result mhr =
            init_531d60_post_base_mov_rsp470_rdx_532ab2_u1(mov_head_cells);
        if (!mhr.ok) {
            return out;
        }
    } else {
        void* desc3_head = desc3_ptr;
        std::memcpy(&desc3.zero[0], &desc3_head, sizeof(void*));
    }

    Init531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1Cells vdqa_cells =
        hooks.vmovdqa_rsp478_xmm0_532aba_u1_cells;
    if (vdqa_cells.dest_out == nullptr) {
        vdqa_cells.dest_out = &desc3.zero[8];
    }
    if (vdqa_cells.xmm0_in == nullptr) {
        vdqa_cells.xmm0_in = xmm0_buf;
    }
    if (!hooks.merge_vmovdqa_rsp478_xmm0_532aba_u1_ran) {
        const Init531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1Result vdr =
            init_531d60_post_base_vmovdqa_rsp478_xmm0_532aba_u1(vdqa_cells);
        if (!vdr.ok) {
            return out;
        }
    } else {
        std::memcpy(&desc3.zero[8], xmm0_buf, 16);
    }

    Init531d60PostBaseMovRsp488Rax_532ac3_U1Cells mov488_cells =
        hooks.mov_rsp488_rax_532ac3_u1_cells;
    if (mov488_cells.thunk_out == nullptr) {
        mov488_cells.thunk_out = reinterpret_cast<void**>(&desc3.zero[0x18]);
    }
    if (mov488_cells.rax_in == nullptr) {
        mov488_cells.rax_in = rax_thunk1;
    }
    if (!hooks.merge_mov_rsp488_rax_532ac3_u1_ran) {
        const Init531d60PostBaseMovRsp488Rax_532ac3_U1Result m488r =
            init_531d60_post_base_mov_rsp488_rax_532ac3_u1(mov488_cells);
        if (!m488r.ok) {
            return out;
        }
    } else {
        std::memcpy(&desc3.zero[0x18], &rax_thunk1, sizeof(void*));
    }

    Init531d60PostBaseMovRsp4b0_Rsi_532acb_U1Cells mov4b0_cells =
        hooks.mov_rsp4b0_rsi_532acb_u1_cells;
    if (mov4b0_cells.vtable_stack_out == nullptr) {
        mov4b0_cells.vtable_stack_out = &desc3.vtable;
    }
    if (mov4b0_cells.rsi_in == nullptr) {
        mov4b0_cells.rsi_in = lea_rsi_out;
        if (mov4b0_cells.rsi_in == nullptr) {
            mov4b0_cells.rsi_in = reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                kInit531d60PostBaseContinueEVtable2Rva));
        }
    }
    if (!hooks.merge_mov_rsp4b0_rsi_532acb_u1_ran) {
        const Init531d60PostBaseMovRsp4b0_Rsi_532acb_U1Result m4r =
            init_531d60_post_base_mov_rsp4b0_rsi_532acb_u1(mov4b0_cells);
        if (!m4r.ok) {
            return out;
        }
    } else {
        desc3.vtable = reinterpret_cast<void*>(
            static_cast<std::uintptr_t>(kInit531d60PostBaseContinueEVtable2Rva));
    }

    // 0x532ad3 lea rsi,[0x2efda50] — name struct 3 (RIP-rel pin)
    pin(out, ContinueEUnit::lea_rsi_2efda50);
    ++out.lea_rip_count;

    // 0x532ada mov [rsp+0x4B8],rcx — desc3[0x48] = 0x538870 (thunk 2)
    desc3.field_48 = kInit531d60PostBaseContinueEThunk2Rva;
    pin(out, ContinueEUnit::mov_rsp4b8_rcx);

    // 0x532ae2 call 0xa54920(parent, name=0x2efda50, descriptor=&desc3)
    if (hooks.call_a54920 != nullptr) {
        hooks.call_a54920(parent3,
                          reinterpret_cast<void*>(
                              static_cast<std::uintptr_t>(kInit531d60PostBaseContinueEName3Rva)),
                          desc3_ptr);
    }
    pin(out, ContinueEUnit::call_a54920_3);
    ++out.call_a54920_count;

    // 0x532ae7 mov rax,[rsp+0x4B0] — reload vtable 2
    pin(out, ContinueEUnit::mov_rax_rsp4b0);

    // 0x532aef mov rdi,rbx — this = &desc3[0x20]
    pin(out, ContinueEUnit::mov_rdi_rbx);

    // 0x532af2 call [rax+0x18] — vcall slot-3 → 0x5388c0 (bare ret, elided).
    // Resolution: *(0x15c8e18 + 0x18) = 0x5388c0 = ret (E537).
    pin(out, ContinueEUnit::vcall_noop_3);
    ++out.vcall_noop_count;

    // ---- Block 4: resolved 0x54b250 call via 0x2eff450 ----
    // Resolution per E538: *(0x2eff450) is a 0x460-byte object whose vtable
    // is 0x15c9830.  Slot 1 (offset +0x8) = 0x54b250.  The call is:
    //   0x54b250(*(0x2eff450), esi=0, edx=2)

    // 0x532af5 lea rax,[0x2eff450] — BSS vtable-object cell
    pin(out, ContinueEUnit::lea_rax_2eff450);
    ++out.lea_rip_count;

    // 0x532afc xor esi,esi — arg1 = 0
    pin(out, ContinueEUnit::xor_esi_esi);

    // 0x532afe mov edx,2 — arg2 = 2
    pin(out, ContinueEUnit::mov_edx_2);

    // 0x532b03 mov rdi,[rax] — rdi = *0x2eff450 (object pointer)
    void* obj_2eff450 = nullptr;
    if (hooks.cell_2eff450 != nullptr) {
        obj_2eff450 = *hooks.cell_2eff450;
    }
    pin(out, ContinueEUnit::mov_rdi_rax);

    // 0x532b06 mov rax,[rdi] — rax = object->vtable (0x15c9830)
    // E538: the vtable pointer stored in the object = 0x15c9830, slot 1 = 0x54b250.
    pin(out, ContinueEUnit::mov_rax_rdi);

    // 0x532b09 call [rax+0x8] — vcall slot-1 → 0x54b250(this_obj, esi=0, edx=2).
    // E547: prefer the lifted wrapper body when an engine contract is supplied.
    if (hooks.stat_engine != nullptr) {
        out.stat_query = stat_query_54b250(obj_2eff450, 0, 2, *hooks.stat_engine);
        out.stat_query_ran = true;
    } else if (hooks.call_54b250 != nullptr) {
        hooks.call_54b250(obj_2eff450, 0, 2);
    }
    pin(out, ContinueEUnit::vcall_rax08);
    ++out.vcall_54b250_count;

    // Final validation (u1 / vcall18 / a54920_532a62 merges add to ctor/vcall/stamp totals).
    const int expect_a54920 =
        1 + (hooks.merge_a54920_u1_ran ? 0 : 1) +
        (hooks.merge_a54920_532a62_u1_ran ? 0 : 1);
    const int expect_vcall_noop =
        1 + (hooks.merge_vcall18_532a17_ran ? 0 : 1) +
        (hooks.merge_vcall18_532a72_u1_ran ? 0 : 1);
    const int expect_stamp = hooks.merge_a54920_u1_ran ? 0 : 1;
    const int expect_lea_rip =
        2 + (hooks.merge_a54920_u1_ran ? 0 : 3) +
        (hooks.merge_a54920_532a62_u1_ran ? 0 : 1) +
        (hooks.merge_lea_5388e0_532a7d_u1_ran ? 0 : 1) +
        (hooks.merge_lea_rsi_15c8e18_532a8c_u1_ran ? 0 : 1) +
        (hooks.merge_lea_538870_532aa3_u1_ran ? 0 : 1);
    const int expect_vmovq = hooks.merge_vmovq_532a75_u1_ran ? 0 : 1;
    out.a54920_hook_supplied = (hooks.call_a54920 != nullptr);
    out.ok = (out.unit_executed == kInit531d60PostBaseContinueEUnitCount) &&
             (out.call_a54920_count == expect_a54920) &&
             (out.vcall_noop_count == expect_vcall_noop) &&
             (out.vcall_54b250_count == 1) && (out.stamp_count == expect_stamp) &&
             (out.lea_rip_count == expect_lea_rip) && (out.vmovq_count == expect_vmovq) &&
             (out.unit_counts[0] == 1) &&
             (out.unit_counts[kInit531d60PostBaseContinueEUnitCount - 1] == 1);
    return out;
}

}  // namespace sotc
