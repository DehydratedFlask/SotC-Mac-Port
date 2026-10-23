// Init `0x531d60` continue E — OBS `0x5329ae..0x532b0c` (350 B, 53 consecutive units).
//
// This is the BRANCH-TARGET block of batch C's `je 0x5329ae` (je TAKEN).
// Straight-line run: three save-data subsystem constructors (0xa54920), each
// preceded by a vmovaps-zeroed 0x50 B stack descriptor with vtable pointer at
// +0x40 and self-pointer at +0x20, followed by a vcall [rax+0x18] through the
// descriptor's vtable; the first two vcalls resolve to bare `ret` (0x538200 via
// 0x15c8d98+0x18, elided), the third also to bare `ret` (0x5388c0 via
// 0x15c8e18+0x18, elided).  The block ends with a resolved vcall [rax+0x8]
// through *(0x2eff450) → vtable 0x15c9830 slot-1 = 0x54b250(this, 0, 2).
//
// A single `mov [rax],r12b` (unit 11, 0x532a04) stamps the boss-flag byte
// cell at 0x2dcdf14 with the incoming r12b.
//
// Structure:
//   Block 1 (0x5329ae..0x532a19):  name 0x2efd9a0 ("save_%s_max_count"),
//       vtable 0x15c8d98 via rbx, boss-flag stamp, call 0xa54920, vcall no-op.
//   Block 2 (0x532a1a..0x532a74):  name 0x2efd9f8 ("save_%s_saves_used"),
//       vtable 0x15c8d98 (rbx reused), call 0xa54920, vcall no-op.
//   Block 3 (0x532a75..0x532af4):  vmovq from 0x15ff528 (8 B load),
//       thunks 0x5388e0/0x538870 stored into descriptor, vtable 0x15c8e18,
//       name 0x2efda50 ("save_%s_mem_used"), call 0xa54920, vcall no-op.
//   Block 4 (0x532af5..0x532b0c):  lea 0x2eff450, xor esi,esi, mov edx,2,
//       call [rax+0x8] → 0x54b250(*(0x2eff450), 0, 2).
//
// Claimed: 53-unit span, three rel32 call 0xa54920 targets, three vcall+0x18
// no-op sites with resolution pins, one vcall+0x8 site with resolved target
// 0x54b250, nine RIP-relative lea targets, one vmovq target, the mov [rax],r12b
// stamp site.
//
// NOT modelled: 0xa54920 body (contract/hook with visible status); 0x54b250
// body; the three provably-ret vcall bodies (0x538200, 0x5388c0); the value
// held by 0x32f79a8 (addressed in batch D); r13/r12b provenance; the descriptor
// field semantics beyond address pinning; any game save-data side-effect.
//
// G1 PARTIAL — 0xa54920 is a hook/contract, nothing ran against the game.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseContinueERva = 0x5329ae;
inline constexpr std::size_t kInit531d60PostBaseContinueESize = 350;
inline constexpr std::uint64_t kInit531d60PostBaseContinueEEndRva = 0x532b0c;

// Direct-call rel32 targets.
inline constexpr std::uint64_t kInit531d60PostBaseContinueECtorRva = 0xa54920;

// No-op vcall resolution pins (slot +0x18 → bare ret).
inline constexpr std::uint64_t kInit531d60PostBaseContinueENoopVcall1TargetRva = 0x538200;
inline constexpr std::uint64_t kInit531d60PostBaseContinueENoopVcall2TargetRva = 0x5388c0;

// Resolved vcall target (slot +0x8 → 0x54b250).
inline constexpr std::uint64_t kInit531d60PostBaseContinueEResolvedVcallRva = 0x54b250;

// RIP-relative lea targets.
inline constexpr std::uint64_t kInit531d60PostBaseContinueEBossFlagRva = 0x2dcdf14;
inline constexpr std::uint64_t kInit531d60PostBaseContinueEName1Rva = 0x2efd9a0;
inline constexpr std::uint64_t kInit531d60PostBaseContinueEName2Rva = 0x2efd9f8;
inline constexpr std::uint64_t kInit531d60PostBaseContinueEName3Rva = 0x2efda50;
inline constexpr std::uint64_t kInit531d60PostBaseContinueEVtable1Rva = 0x15c8d98;
inline constexpr std::uint64_t kInit531d60PostBaseContinueEVtable2Rva = 0x15c8e18;
inline constexpr std::uint64_t kInit531d60PostBaseContinueEThunk1Rva = 0x5388e0;
inline constexpr std::uint64_t kInit531d60PostBaseContinueEThunk2Rva = 0x538870;
inline constexpr std::uint64_t kInit531d60PostBaseContinueECell2eff450Rva = 0x2eff450;

// vmovq xmm load source (8 B from a 16 B ptr array).
inline constexpr std::uint64_t kInit531d60PostBaseContinueEVmovqSrcRva = 0x15ff528;

// Vtable for the 0x2eff450 object (slot-1 = 0x54b250, resolved per E538).
inline constexpr std::uint64_t kInit531d60PostBaseContinueEVtable2eff450Rva = 0x15c9830;

// Unit identity for the 53 pinned instructions of the claimed run.
enum class ContinueEUnit : int {
    lea_rax_2dcdf14 = 0,       // 0x5329ae
    mov_rdi_r13,               // 0x5329b5
    lea_rsi_2efd9a0,           // 0x5329b9
    lea_rdx_rsp520,            // 0x5329c0
    vxorps_ymm0,               // 0x5329c8
    lea_rbx_15c8d98,           // 0x5329cc
    movq_rsp550_0,             // 0x5329d3
    lea_r14_rsp540,            // 0x5329df
    vmovaps_rsp520_ymm0,       // 0x5329e7
    mov_rsp560_rbx,            // 0x5329f0
    movq_rsp568_0,             // 0x5329f8
    mov_rax_r12b,              // 0x532a04  (stamp boss flag)
    call_a54920_1,             // 0x532a07
    mov_rax_rsp560_1,          // 0x532a0c
    mov_rdi_r14_1,             // 0x532a14
    vcall_noop_1,              // 0x532a17  (rax+0x18 → 0x538200 ret)
    mov_rdi_r13_2,             // 0x532a1a
    lea_rsi_2efd9f8,           // 0x532a1e
    lea_rdx_rsp4c0,            // 0x532a25
    vxorps_ymm0_2,             // 0x532a2d
    movq_rsp4f0_0,             // 0x532a31
    lea_r14_rsp4e0,            // 0x532a3d
    vmovaps_rsp4c0_ymm0,       // 0x532a45
    mov_rsp500_rbx,            // 0x532a4e
    movq_rsp508_0,             // 0x532a56
    call_a54920_2,             // 0x532a62
    mov_rax_rsp500,            // 0x532a67
    mov_rdi_r14_2,             // 0x532a6f
    vcall_noop_2,              // 0x532a72  (rax+0x18 → 0x538200 ret)
    vmovq_xmm0_15ff528,        // 0x532a75
    lea_rax_5388e0,            // 0x532a7d
    lea_rdx_rsp470,            // 0x532a84
    lea_rsi_15c8e18,           // 0x532a8c
    movq_rsp4a0_0,             // 0x532a93
    mov_rdi_r13_3,             // 0x532a9f
    lea_rcx_538870,            // 0x532aa3
    lea_rbx_rsp490,            // 0x532aaa
    mov_rsp470_rdx,            // 0x532ab2
    vmovdqa_rsp478_xmm0,       // 0x532aba
    mov_rsp488_rax,            // 0x532ac3
    mov_rsp4b0_rsi,            // 0x532acb
    lea_rsi_2efda50,           // 0x532ad3
    mov_rsp4b8_rcx,            // 0x532ada
    call_a54920_3,             // 0x532ae2
    mov_rax_rsp4b0,            // 0x532ae7
    mov_rdi_rbx,               // 0x532aef
    vcall_noop_3,              // 0x532af2  (rax+0x18 → 0x5388c0 ret)
    lea_rax_2eff450,           // 0x532af5
    xor_esi_esi,               // 0x532afc
    mov_edx_2,                 // 0x532afe
    mov_rdi_rax,               // 0x532b03
    mov_rax_rdi,               // 0x532b06
    vcall_rax08,               // 0x532b09  (rax+0x8 → 0x54b250)
    count
};

inline constexpr int kInit531d60PostBaseContinueEUnitCount =
    static_cast<int>(ContinueEUnit::count);
static_assert(kInit531d60PostBaseContinueEUnitCount == 53,
              "53 units in 0x5329ae..0x532b0c");

struct ContinueEUnitSpan {
    std::uint64_t rva = 0;
    std::uint8_t size = 0;
    const char* mnemonic = "";
};

// OBS table derived from local/re/eboot.elf; sizes sum to kSize (350 B).
inline constexpr std::array<ContinueEUnitSpan, kInit531d60PostBaseContinueEUnitCount>
    kInit531d60PostBaseContinueEUnits{{
        {0x5329AE, 7, "lea rax,[0x2dcdf14]"},
        {0x5329B5, 4, "mov rdi,[r13]"},
        {0x5329B9, 7, "lea rsi,[0x2efd9a0]"},
        {0x5329C0, 8, "lea rdx,[rsp+0x520]"},
        {0x5329C8, 4, "vxorps ymm0,ymm0,ymm0"},
        {0x5329CC, 7, "lea rbx,[0x15c8d98]"},
        {0x5329D3, 12, "mov qword [rsp+0x550],0"},
        {0x5329DF, 8, "lea r14,[rsp+0x540]"},
        {0x5329E7, 9, "vmovaps [rsp+0x520],ymm0"},
        {0x5329F0, 8, "mov [rsp+0x560],rbx"},
        {0x5329F8, 12, "mov qword [rsp+0x568],0"},
        {0x532A04, 3, "mov [rax],r12b"},
        {0x532A07, 5, "call 0xa54920"},
        {0x532A0C, 8, "mov rax,[rsp+0x560]"},
        {0x532A14, 3, "mov rdi,r14"},
        {0x532A17, 3, "call [rax+0x18]"},
        {0x532A1A, 4, "mov rdi,[r13]"},
        {0x532A1E, 7, "lea rsi,[0x2efd9f8]"},
        {0x532A25, 8, "lea rdx,[rsp+0x4C0]"},
        {0x532A2D, 4, "vxorps ymm0,ymm0,ymm0"},
        {0x532A31, 12, "mov qword [rsp+0x4F0],0"},
        {0x532A3D, 8, "lea r14,[rsp+0x4E0]"},
        {0x532A45, 9, "vmovaps [rsp+0x4C0],ymm0"},
        {0x532A4E, 8, "mov [rsp+0x500],rbx"},
        {0x532A56, 12, "mov qword [rsp+0x508],0"},
        {0x532A62, 5, "call 0xa54920"},
        {0x532A67, 8, "mov rax,[rsp+0x500]"},
        {0x532A6F, 3, "mov rdi,r14"},
        {0x532A72, 3, "call [rax+0x18]"},
        {0x532A75, 8, "vmovq xmm0,[0x15ff528]"},
        {0x532A7D, 7, "lea rax,[0x5388e0]"},
        {0x532A84, 8, "lea rdx,[rsp+0x470]"},
        {0x532A8C, 7, "lea rsi,[0x15c8e18]"},
        {0x532A93, 12, "mov qword [rsp+0x4A0],0"},
        {0x532A9F, 4, "mov rdi,[r13]"},
        {0x532AA3, 7, "lea rcx,[0x538870]"},
        {0x532AAA, 8, "lea rbx,[rsp+0x490]"},
        {0x532AB2, 8, "mov [rsp+0x470],rdx"},
        {0x532ABA, 9, "vmovdqa [rsp+0x478],xmm0"},
        {0x532AC3, 8, "mov [rsp+0x488],rax"},
        {0x532ACB, 8, "mov [rsp+0x4B0],rsi"},
        {0x532AD3, 7, "lea rsi,[0x2efda50]"},
        {0x532ADA, 8, "mov [rsp+0x4B8],rcx"},
        {0x532AE2, 5, "call 0xa54920"},
        {0x532AE7, 8, "mov rax,[rsp+0x4B0]"},
        {0x532AEF, 3, "mov rdi,rbx"},
        {0x532AF2, 3, "call [rax+0x18]"},
        {0x532AF5, 7, "lea rax,[0x2eff450]"},
        {0x532AFC, 2, "xor esi,esi"},
        {0x532AFE, 5, "mov edx,2"},
        {0x532B03, 3, "mov rdi,[rax]"},
        {0x532B06, 3, "mov rax,[rdi]"},
        {0x532B09, 3, "call [rax+0x8]"},
    }};

// Host stand-ins for the pinned operands. Every field optional: a null cell
// or hook skips that host-side operation (the unit is still counted).
struct Init531d60PostBaseContinueEHooks {
    // Incoming registers at je-taken entry (provenance upstream, NOT modelled):
    // r13 holds a stable init pointer (parent arg for all three save-data ctors).
    const void* incoming_r13 = nullptr;
    // r12b is the byte value stamped into 0x2dcdf14 at unit 11.
    std::uint8_t incoming_r12b = 0;

    // Boss-flag cell (host-supplied byte written at 0x532a04).
    // null => stamp is counted but no write occurs.
    std::uint8_t* boss_flag_2dcdf14 = nullptr;

    // Save-data subsystem constructor: 0xa54920(void* parent, void* name_struct,
    // void* descriptor).  Called three times with distinct name structs and
    // stack descriptors.  null => no-op (call still counted as visible).
    void (*call_a54920)(void* parent, void* name_struct, void* descriptor) = nullptr;

    // Resolved vcall slot-1 through *(0x2eff450) — 0x54b250(this_obj, esi=0,
    // edx=2).  null => no-op (call still counted).
    void (*call_54b250)(void* this_obj, std::uint32_t esi, std::uint32_t edx) = nullptr;

    // 0x2eff450 cell content stand-in (host-supplied object pointer).
    // null => deref skipped, rdi=null, the rest still counted.
    void* const* cell_2eff450 = nullptr;
};

struct Init531d60PostBaseContinueEResult {
    bool ok = false;
    // Per-unit execution counters (all must be 1 on the modelled path).
    std::array<int, kInit531d60PostBaseContinueEUnitCount> unit_counts{};
    int unit_executed = 0;          // expect 53
    int call_a54920_count = 0;      // expect 3 (contract/hook with visible status)
    int vcall_noop_count = 0;       // expect 3 (elided bare ret; counted, no dispatch)
    int vcall_54b250_count = 0;     // expect 1 (resolved slot-1)
    int stamp_count = 0;            // expect 1 (mov [rax],r12b @0x532a04)
    int lea_rip_count = 0;          // expect 9 RIP-relative lea
    int vmovq_count = 0;            // expect 1 (vmovq xmm0,[0x15ff528])

    // Stamp cell observation (before/after the r12b write, when boss_flag is supplied).
    std::uint8_t boss_flag_before = 0;
    std::uint8_t boss_flag_after = 0;

    // Per-call descriptor debug (addresses of the three stack descriptors).
    // These are the rdx values passed to the three 0xa54920 calls.
    // The callee bodies are NOT modelled.
    bool a54920_hook_supplied = false;  // true when call_a54920 was non-null

    // Pinned end address (constant, not measured control flow).
    std::uint64_t end_rva = kInit531d60PostBaseContinueEEndRva;
};

// Runs the OBS `0x5329ae..0x532b0c` unit sequence: the branch-target block
// entered when batch C's `je 0x5329ae` is taken.
[[nodiscard]] Init531d60PostBaseContinueEResult init_531d60_post_base_continue_e(
    const Init531d60PostBaseContinueEHooks& hooks = {}) noexcept;

}  // namespace sotc
