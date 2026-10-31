#include "init_531d60_post_base_continue_e.hpp"

#include "init_531d60_post_base_a54920_u1.hpp"

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

    // 0x532a0c mov rax,[rsp+0x560] — reload vtable (stack from u1 prep NOT modelled)
    pin(out, ContinueEUnit::mov_rax_rsp560_1);

    // 0x532a14 mov rdi,r14 — this = &desc1[0x20]
    pin(out, ContinueEUnit::mov_rdi_r14_1);

    // 0x532a17 call [rax+0x18] — vcall slot-3 → 0x538200 (bare ret, elided).
    // Resolution: *(0x15c8d98 + 0x18) = 0x538200 = ret (E537).
    pin(out, ContinueEUnit::vcall_noop_1);
    ++out.vcall_noop_count;

    // ---- Block 2: save-data ctor 2 (name 0x2efd9f8, vtable 0x15c8d98, rbx reused) ----

    // 0x532a1a mov rdi,[r13] — parent arg
    void* parent2 = const_cast<void*>(hooks.incoming_r13);
    pin(out, ContinueEUnit::mov_rdi_r13_2);

    // 0x532a1e lea rsi,[0x2efd9f8] — name struct 2 (RIP-rel pin)
    pin(out, ContinueEUnit::lea_rsi_2efd9f8);
    ++out.lea_rip_count;

    // 0x532a25 lea rdx,[rsp+0x4C0] — descriptor 2 on stack
    alignas(32) SaveDataDescriptor desc2{};
    void* desc2_ptr = &desc2;
    pin(out, ContinueEUnit::lea_rdx_rsp4c0);

    // 0x532a2d vxorps ymm0,ymm0,ymm0
    pin(out, ContinueEUnit::vxorps_ymm0_2);

    // 0x532a31 mov qword [rsp+0x4F0],0 — desc2[0x30] = 0
    desc2.field_30 = 0;
    pin(out, ContinueEUnit::movq_rsp4f0_0);

    // 0x532a3d lea r14,[rsp+0x4E0] — self-ptr = &desc2[0x20]
    desc2.self_ptr = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&desc2) + 0x20);
    pin(out, ContinueEUnit::lea_r14_rsp4e0);

    // 0x532a45 vmovaps [rsp+0x4C0],ymm0 — zero desc2[0..0x1F]
    std::memset(desc2.zero, 0, sizeof(desc2.zero));
    pin(out, ContinueEUnit::vmovaps_rsp4c0_ymm0);

    // 0x532a4e mov [rsp+0x500],rbx — desc2[0x40] = vtable (rbx still 0x15c8d98)
    desc2.vtable = reinterpret_cast<void*>(
        static_cast<std::uintptr_t>(kInit531d60PostBaseContinueEVtable1Rva));
    pin(out, ContinueEUnit::mov_rsp500_rbx);

    // 0x532a56 mov qword [rsp+0x508],0 — desc2[0x48] = 0
    desc2.field_48 = 0;
    pin(out, ContinueEUnit::movq_rsp508_0);

    // 0x532a62 call 0xa54920(parent, name=0x2efd9f8, descriptor=&desc2)
    if (hooks.call_a54920 != nullptr) {
        hooks.call_a54920(parent2,
                          reinterpret_cast<void*>(
                              static_cast<std::uintptr_t>(kInit531d60PostBaseContinueEName2Rva)),
                          desc2_ptr);
    }
    pin(out, ContinueEUnit::call_a54920_2);
    ++out.call_a54920_count;

    // 0x532a67 mov rax,[rsp+0x500]
    pin(out, ContinueEUnit::mov_rax_rsp500);

    // 0x532a6f mov rdi,r14
    pin(out, ContinueEUnit::mov_rdi_r14_2);

    // 0x532a72 call [rax+0x18] — vcall slot-3 → 0x538200 (bare ret, elided).
    pin(out, ContinueEUnit::vcall_noop_2);
    ++out.vcall_noop_count;

    // ---- Block 3: save-data ctor 3 (name 0x2efda50, vtable 0x15c8e18) ----
    // This descriptor setup is more elaborate: stores self-ptr at +0x00,
    // loads 8 B from 0x15ff528 into xmm0 and stores at +0x08, stores
    // thunk 0x5388e0 at +0x18, vtable 0x15c8e18 at +0x40, thunk 0x538870
    // at +0x48.

    // 0x532a75 vmovq xmm0,[0x15ff528] — load 8 B from the ptr array
    // (original semantics: load for descriptor3[0x08]; value NOT modelled).
    alignas(16) std::uint8_t xmm0_buf[16]{};
    // The vmovq reads 8 bytes at the source RVA; in a native build we
    // can't read the actual file, so we use a zero stand-in.
    // The original is a link-time constant in rodata; the actual value
    // is not needed for the modelled contract (the descriptor shape is
    // pinned, not the data semantics).
    pin(out, ContinueEUnit::vmovq_xmm0_15ff528);
    ++out.vmovq_count;

    // 0x532a7d lea rax,[0x5388e0] — thunk 1 (ret no-op)
    pin(out, ContinueEUnit::lea_rax_5388e0);
    ++out.lea_rip_count;

    // 0x532a84 lea rdx,[rsp+0x470] — descriptor 3 on stack
    alignas(32) SaveDataDescriptor desc3{};
    void* desc3_ptr = &desc3;
    pin(out, ContinueEUnit::lea_rdx_rsp470);

    // 0x532a8c lea rsi,[0x15c8e18] — vtable 2 (slot-3 no-op source)
    pin(out, ContinueEUnit::lea_rsi_15c8e18);
    ++out.lea_rip_count;

    // 0x532a93 mov qword [rsp+0x4A0],0 — desc3[0x30] = 0
    desc3.field_30 = 0;
    pin(out, ContinueEUnit::movq_rsp4a0_0);

    // 0x532a9f mov rdi,[r13] — parent arg
    void* parent3 = const_cast<void*>(hooks.incoming_r13);
    pin(out, ContinueEUnit::mov_rdi_r13_3);

    // 0x532aa3 lea rcx,[0x538870] — thunk 2 (ret no-op)
    pin(out, ContinueEUnit::lea_rcx_538870);
    ++out.lea_rip_count;

    // 0x532aaa lea rbx,[rsp+0x490] — &desc3[0x20] (self-ptr, stored as rbx for
    // later vcall dispatch)
    desc3.self_ptr = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&desc3) + 0x20);
    pin(out, ContinueEUnit::lea_rbx_rsp490);

    // 0x532ab2 mov [rsp+0x470],rdx — desc3[0x00] = &desc3 (self-pointer at head)
    // Original: stores rdx (the descriptor base address) into desc3[0x00].
    // We store the actual desc3_ptr.
    {
        void* desc3_head = desc3_ptr;
        std::memcpy(&desc3.zero[0], &desc3_head, sizeof(void*));
    }
    pin(out, ContinueEUnit::mov_rsp470_rdx);

    // 0x532aba vmovdqa [rsp+0x478],xmm0 — store xmm0 at desc3[0x08]
    // (value from vmovq; stand-in zero used here).
    std::memcpy(&desc3.zero[8], xmm0_buf, 16);
    pin(out, ContinueEUnit::vmovdqa_rsp478_xmm0);

    // 0x532ac3 mov [rsp+0x488],rax — desc3[0x18] = 0x5388e0 (thunk 1)
    {
        void* thunk1_val = reinterpret_cast<void*>(
            static_cast<std::uintptr_t>(kInit531d60PostBaseContinueEThunk1Rva));
        std::memcpy(&desc3.zero[0x18], &thunk1_val, sizeof(void*));
    }
    pin(out, ContinueEUnit::mov_rsp488_rax);

    // 0x532acb mov [rsp+0x4B0],rsi — desc3[0x40] = 0x15c8e18 (vtable 2)
    desc3.vtable = reinterpret_cast<void*>(
        static_cast<std::uintptr_t>(kInit531d60PostBaseContinueEVtable2Rva));
    pin(out, ContinueEUnit::mov_rsp4b0_rsi);

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

    // Final validation.
    out.a54920_hook_supplied = (hooks.call_a54920 != nullptr);
    out.ok = (out.unit_executed == kInit531d60PostBaseContinueEUnitCount) &&
             (out.call_a54920_count == 3) && (out.vcall_noop_count == 3) &&
             (out.vcall_54b250_count == 1) && (out.stamp_count == 1) &&
             (out.lea_rip_count == 9) && (out.vmovq_count == 1) &&
             (out.unit_counts[0] == 1) &&
             (out.unit_counts[kInit531d60PostBaseContinueEUnitCount - 1] == 1);
    return out;
}

}  // namespace sotc
