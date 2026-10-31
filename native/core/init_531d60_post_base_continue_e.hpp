// Init `0x531d60` continue E — OBS `0x532a93..0x532b0c` (121 B, 20 consecutive units).
//
// Branch-target block of batch C's `je 0x5329ae` (je TAKEN). Entry @ `0x5329ae`,
// first `call 0xa54920` @ `0x532a07` via `a54920_u1` (E520), first vcall
// `[rax+0x18]` @ `0x532a17` via `vcall18_532a17` (E521), second
// `call 0xa54920` @ `0x532a62` via `a54920_532a62_u1` (E522), second vcall
// `[rax+0x18]` @ `0x532a72` via `vcall18_532a72_u1` (E523), vmovq
// `[0x15ff528]` @ `0x532a75` via `vmovq_532a75_u1` (E524), lea
// `[0x5388e0]` @ `0x532a7d` via `lea_5388e0_532a7d_u1` (E525), lea
// `[rsp+0x470]` @ `0x532a84` via `lea_rsp470_532a84_u1` (E526), lea
// `[0x15c8e18]` @ `0x532a8c` via `lea_rsi_15c8e18_532a8c_u1` (E527); this module pins
// the post-lea-rsi tail through `0x532b0c`.
//
// Full ELF block `0x5329ae..0x532b0c` (350 B) including demoted u1 prep.
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
//   Block 1 (0x5329ae..0x532a19):  u1 prep + call + vcall18_532a17 peel (NOT here).
//   Block 2 (0x532a1a..0x532a74):  a54920_532a62_u1 + vcall18_532a72_u1 peels (NOT here).
//   Block 3 (0x532a84..0x532af4):  vmovq_532a75_u1 + lea_5388e0_532a7d_u1 +
//       lea_rsp470_532a84_u1 peels (NOT here);
//       stack/thunk prep, thunks 0x5388e0/0x538870 stored into descriptor,
//       vtable 0x15c8e18,
//       name 0x2efda50 ("save_%s_mem_used"), call 0xa54920, vcall no-op.
//   Block 4 (0x532af5..0x532b0c):  lea 0x2eff450, xor esi,esi, mov edx,2,
//       call [rax+0x8] → 0x54b250(*(0x2eff450), 0, 2).
//
// Claimed: 20-unit pinned span (plus u1 + vcall18 + a54920_532a62 + vcall18_532a72 +
// vmovq_532a75 + lea_5388e0_532a7d_u1 + lea_rsp470_532a84_u1 + lea_rsi_15c8e18_532a8c_u1
// merges), three rel32 call
// 0xa54920 targets (one via u1), three vcall+0x18
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

#include "init_531d60_post_base_a54920_532a62_u1.hpp"
#include "init_531d60_post_base_a54920_u1.hpp"
#include "init_531d60_post_base_vcall18_532a17.hpp"
#include "init_531d60_post_base_vcall18_532a72_u1.hpp"
#include "init_531d60_post_base_lea_5388e0_532a7d_u1.hpp"
#include "init_531d60_post_base_lea_rsp470_532a84_u1.hpp"
#include "init_531d60_post_base_lea_rsi_15c8e18_532a8c_u1.hpp"
#include "init_531d60_post_base_vmovq_532a75_u1.hpp"
#include "stat_query_54b250.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseContinueERva = 0x532a93;
inline constexpr std::size_t kInit531d60PostBaseContinueESize = 121;
inline constexpr std::uint64_t kInit531d60PostBaseContinueEEndRva = 0x532b0c;
inline constexpr std::size_t kInit531d60PostBaseContinueEFullBlockSize = 350;

static_assert(kInit531d60PostBaseA54920U1EndRva ==
                  kInit531d60PostBaseVcall18532a17Rva,
              "a54920 u1 end equals vcall18_532a17 start");
static_assert(kInit531d60PostBaseVcall18532a17EndRva ==
                  kInit531d60PostBaseA54920_532a62_U1PrepRva,
              "vcall18_532a17 end equals a54920_532a62 prep");
static_assert(kInit531d60PostBaseA54920_532a62_U1EndRva ==
                  kInit531d60PostBaseVcall18_532a72_U1PrepRva,
              "a54920_532a62_u1 end equals vcall18_532a72 prep");
static_assert(kInit531d60PostBaseVcall18_532a72_U1EndRva ==
                  kInit531d60PostBaseVmovq532a75_U1Rva,
              "vcall18_532a72_u1 end equals vmovq_532a75_u1 start");
static_assert(kInit531d60PostBaseVmovq532a75_U1EndRva ==
                  kInit531d60PostBaseLea5388e0_532a7d_U1Rva,
              "vmovq_532a75_u1 end equals lea_5388e0_532a7d_u1 start");
static_assert(kInit531d60PostBaseLea5388e0_532a7d_U1EndRva ==
                  kInit531d60PostBaseLeaRsp470_532a84_U1Rva,
              "lea_5388e0_532a7d_u1 end equals lea_rsp470_532a84_u1 start");
static_assert(kInit531d60PostBaseLeaRsp470_532a84_U1EndRva ==
                  kInit531d60PostBaseLeaRsi15c8e18_532a8c_U1Rva,
              "lea_rsp470_532a84_u1 end equals lea_rsi_15c8e18_532a8c_u1 start");
static_assert(kInit531d60PostBaseLeaRsi15c8e18_532a8c_U1EndRva == kInit531d60PostBaseContinueERva,
              "lea_rsi_15c8e18_532a8c_u1 end equals continue E pinned start");

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

// Unit identity for the 20 pinned instructions after E527 lea_rsi_15c8e18_532a8c_u1 peel.
enum class ContinueEUnit : int {
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
static_assert(kInit531d60PostBaseContinueEUnitCount == 20,
              "20 units in 0x532a93..0x532b0c");

struct ContinueEUnitSpan {
    std::uint64_t rva = 0;
    std::uint8_t size = 0;
    const char* mnemonic = "";
};

// OBS table derived from local/re/eboot.elf; sizes sum to kSize (121 B).
inline constexpr std::array<ContinueEUnitSpan, kInit531d60PostBaseContinueEUnitCount>
    kInit531d60PostBaseContinueEUnits{{
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
    bool merge_a54920_u1_ran = false;
    bool merge_a54920_532a62_u1_ran = false;

    // First ctor vcall [rax+0x18] @ 0x532a17 (E521 peel). Null → elided no-op.
    void (*call_vslot18_532a17)() = nullptr;
    bool merge_vcall18_532a17_ran = false;

    // Second ctor vcall [rax+0x18] @ 0x532a72 (E523 peel). Null → elided no-op.
    void (*call_vslot18_532a72)() = nullptr;
    bool merge_vcall18_532a72_u1_ran = false;

    // vmovq xmm0,[0x15ff528] @ 0x532a75 (E524 peel). Null cells → counted, zero stand-in.
    Init531d60PostBaseVmovq532a75_U1Cells vmovq_532a75_cells{};
    bool merge_vmovq_532a75_u1_ran = false;

    // lea rax,[0x5388e0] @ 0x532a7d (E525 peel). Null rax_out → counted, default pin.
    Init531d60PostBaseLea5388e0_532a7d_U1Cells lea_5388e0_532a7d_u1_cells{};
    bool merge_lea_5388e0_532a7d_u1_ran = false;

    // lea rdx,[rsp+0x470] @ 0x532a84 (E526 peel). Null cells → counted, no rdx write.
    Init531d60PostBaseLeaRsp470_532a84_U1Cells lea_rsp470_532a84_u1_cells{};
    bool merge_lea_rsp470_532a84_u1_ran = false;

    // lea rsi,[0x15c8e18] @ 0x532a8c (E527 peel). Null rsi_out → counted, default pin.
    Init531d60PostBaseLeaRsi15c8e18_532a8c_U1Cells lea_rsi_15c8e18_532a8c_u1_cells{};
    bool merge_lea_rsi_15c8e18_532a8c_u1_ran = false;

    // Resolved vcall slot-1 through *(0x2eff450) — 0x54b250(this_obj, esi=0,
    // edx=2).  null => no-op (call still counted).
    void (*call_54b250)(void* this_obj, std::uint32_t esi, std::uint32_t edx) = nullptr;

    // 0x2eff450 cell content stand-in (host-supplied object pointer).
    // null => deref skipped, rdi=null, the rest still counted.
    void* const* cell_2eff450 = nullptr;
    // E547: when set, the 0x54b250 dispatch executes the lifted wrapper body with
    // (this = *cell_2eff450, 0, 2) instead of calling the raw hook.
    const StatsEngine54b250* stat_engine = nullptr;
};

struct Init531d60PostBaseContinueEResult {
    bool ok = false;
    // Per-unit execution counters (all must be 1 on the modelled path).
    std::array<int, kInit531d60PostBaseContinueEUnitCount> unit_counts{};
    int unit_executed = 0;          // expect 20 (plus peel merges when run)
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

    // E547: lifted 0x54b250 wrapper result (only when stat_engine was supplied).
    bool stat_query_ran = false;
    StatQuery54b250Result stat_query{};
};

// Runs continue E after optional peel merges:
// pinned `0x532a93..0x532b0c`.
[[nodiscard]] Init531d60PostBaseContinueEResult init_531d60_post_base_continue_e(
    const Init531d60PostBaseContinueEHooks& hooks = {}) noexcept;

}  // namespace sotc
