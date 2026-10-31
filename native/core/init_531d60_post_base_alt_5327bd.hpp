// Init `0x531d60` alternate path OBS `0x5327bd..0x532904` (327 B, 70 consecutive units).
//
// This is the BRANCH-TARGET block of batch A's `je 0x5327bd` at 0x53277d (je TAKEN).
// It models the alternate init path: a gate check at `[0x2dfce70]+0x60`, vtable-heavy
// dispatch through `[0x3305568]` (calls at slots +0x48, +0x28, +0x50, +0x20), a float
// compare against `[0x1014c40]`, a container check at `[0x2ea5170]`, a conditional
// logging sub-block via `0x3e7e0`, and two join paths (success via `jmp 0x532904`;
// error via 36 B preamble setting `sete r14b`).
//
// Also models the exit stub at 0x5327b6 (`xor eax,eax; jmp 0x532c6f`) as an explicit
// "returns 0" path reachable from the base-initialize failure `je 0x5327b6` at 0x531de1.
//
// Structure:
//   Gate (0x5327bd..0x5327d0):     lea [0x2dfce70] → [rax+0x60] → cmp/jne
//   Vtable block (0x5327d1..0x532830): via [0x3305568], 4 indirect vcalls
//   Float compare (0x532831..0x532847): vucomiss vs [0x1014c40] → jbe
//   Container check (0x532848..0x532863): [0x2ea5170] list + flag at +0x128
//   Logging block (0x532864..0x5328bc): format string via 0x3e7e0
//   Success join (0x5328bd..0x5328ce): zero regs, call 0x4c4950, jmp 0x532904
//   Error path (0x5328cf..0x5328df):   set flags, call 0x4c4950
//   36B preamble (0x5328e0..0x532904): read [0x2efe318]+[rax]+[0x2efe340]+0x60, sete
//
// Claimed: 70 units, region sha256
//   `34b848a9426ffd79d2cebc433472d3d8eba3c6b0686fdd9a8dfc5f0d4c7c0206`;
//   direct calls 0xac5260, 0x3e7e0, 0x4c4950(x2); five vcalls through [0x3305568]
//   at slots +0x48/+0x28/+0x50/+0x20 and one vcall [rax] through [0x2efe318];
//   RIP-relative lea targets: 0x2dfce70, 0x3305568, 0x2ea5170, 0x1014db9, 0x1014ded,
//   0x1014d71, 0x2efe318(x2), 0x2efe340; float compare vs 0x1014c40; gate byte at
//   [0x2dfce70]+0x60; join at jmp 0x532904 or sete r14b.
//
// NOT modelled: bodies of 0xac5260, 0x3e7e0, 0x4c4950; all vcall callee bodies;
// the value held by 0x3305568 / 0x2ea5170 / 0x2efe318 / 0x2efe340 / 0x2dfce70 beyond
// the observed offsets; the float value at 0x1014c40; the string data at 0x1014db9/
// 0x1014ded/0x1014d71; incoming r13/r15/rbx provenance; stack frame layout; the
// `mov ah,1` overlapping encoding at 0x5328e0 entry; and any observable game side-effect.
//
// G1 remains PARTIAL.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "lazy_bind_91210.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseAlt5327bdRva = 0x5327BD;
inline constexpr std::size_t  kInit531d60PostBaseAlt5327bdSize = 327;
inline constexpr std::uint64_t kInit531d60PostBaseAlt5327bdEndRva = 0x532904;

// Exit stub (not part of this block's byte span, but modelled explicitly).
inline constexpr std::uint64_t kInit531d60PostBaseAlt5327bdExitStubRva = 0x5327B6;
inline constexpr std::size_t  kInit531d60PostBaseAlt5327bdExitStubSize = 7;

// Direct-call rel32 targets.
inline constexpr std::uint64_t kAlt5327bdCallAc5260Rva = 0xAC5260;
inline constexpr std::uint64_t kAlt5327bdCall3e7e0Rva  = 0x3E7E0;
inline constexpr std::uint64_t kAlt5327bdCall4c4950Rva  = 0x4C4950;

// Gate cell and offset.
inline constexpr std::uint64_t kAlt5327bdGateCellRva = 0x2DFCE70;
inline constexpr std::size_t  kAlt5327bdGateOff = 0x60;

// Object cell for vtable-heavy dispatch.
inline constexpr std::uint64_t kAlt5327bdObj3305568Rva = 0x3305568;

// Float compare constant address.
inline constexpr std::uint64_t kAlt5327bdFloatConstRva = 0x1014C40;

// Container / list check.
inline constexpr std::uint64_t kAlt5327bdContainerRva = 0x2EA5170;

// Logging format string RVAs.
inline constexpr std::uint64_t kAlt5327bdStr1014db9Rva = 0x1014DB9;
inline constexpr std::uint64_t kAlt5327bdStr1014dedRva = 0x1014DED;
inline constexpr std::uint64_t kAlt5327bdStr1014d71Rva = 0x1014D71;

// Preamble cells.
inline constexpr std::uint64_t kAlt5327bdCell2efe318Rva = 0x2EFE318;
inline constexpr std::uint64_t kAlt5327bdCell2efe340Rva = 0x2EFE340;

// Unit identity for the 70 pinned instructions of the claimed run.
enum class Alt5327bdUnit : int {
    // Gate check
    lea_rax_2dfce70 = 0,
    mov_rax_rax60,
    cmp_byte_rax_0,
    jne_5328cf,
    // Object cell & struct
    lea_rbx_3305568,
    mov_rax_3f80,
    lea_rdx_rsp28,
    mov_esi_7,
    mov_rsp28_rax,
    mov_dword_rsp30_2,
    mov_rdi_rbx_1,
    call_ac5260,
    // Vtable dispatch
    mov_rdi_rbx_2,
    mov_rax_rdi_1,
    vcall_rax48,
    mov_rdi_rbx_3,
    xor_esi_esi_1,
    mov_rax_rdi_2,
    vcall_rax28,
    mov_rdi_rbx_4,
    mov_rax_rdi_3,
    vcall_rax50,
    mov_rsi_rbx,
    lea_rdi_rsp570,
    xor_edx_edx,
    mov_rax_rsi,
    vcall_rax20,
    // Float compare
    vmovss_xmm0_rsp58c,
    vucomiss_xmm0_1014c40,
    jbe_5328cf,
    // Container check
    lea_rcx_2ea5170,
    mov_rax_rcx,
    cmp_rax_rcx8,
    je_532864,
    mov_rax_rax0128,
    cmp_byte_rax_0_2,
    je_5328bc,
    // Logging sub-block
    lea_rax_1014db9,
    lea_r9_1014ded,
    lea_rdx_rsp450,
    mov_edi_0,
    mov_esi_1,
    xor_ecx_ecx,
    mov_r8d_1,
    vmovq_xmm0_rax,
    lea_rax_1014d71,
    vmovq_xmm1_rax,
    xor_eax_eax_1,
    vpunpcklqdq,
    vmovdqa_rsp450,
    mov_dword_rsp460_202,
    call_3e7e0,
    // Success cleanup
    xor_edi_edi,
    xor_r12d_r12d,
    call_4c4950_1,
    mov_r15_r13,
    xor_r14d_r14d,
    jmp_532904,
    // Error-catch path
    mov_edi_1,
    call_4c4950_2,
    mov_r15_r13_err,
    mov_r12b_1,
    // 36 B preamble
    mov_rax_2efe318,
    lea_rdi_2efe318,
    xor_esi_esi_2,
    vcall_rax,
    mov_rax_2efe340,
    mov_rax_rax60_2,
    cmp_byte_rax_0_3,
    sete_r14b,
    count
};

inline constexpr int kInit531d60PostBaseAlt5327bdUnitCount =
    static_cast<int>(Alt5327bdUnit::count);
static_assert(kInit531d60PostBaseAlt5327bdUnitCount == 70,
              "70 units in 0x5327bd..0x532904");

struct Alt5327bdUnitSpan {
    std::uint64_t rva = 0;
    std::uint8_t  size = 0;
    const char*   mnemonic = "";
};

// OBS table derived from local/re/eboot.elf; sizes sum to kSize (327 B).
inline constexpr std::array<Alt5327bdUnitSpan, kInit531d60PostBaseAlt5327bdUnitCount>
    kInit531d60PostBaseAlt5327bdUnits{{
        // Gate check
        {0x5327BD, 7,  "lea rax, [0x2dfce70]"},
        {0x5327C4, 4,  "mov rax, [rax+0x60]"},
        {0x5327C8, 3,  "cmp byte [rax], 0"},
        {0x5327CB, 6,  "jne 0x5328cf"},
        // Object cell & struct
        {0x5327D1, 7,  "lea rbx, [0x3305568]"},
        {0x5327D8, 10, "mov rax, 0x3f80000000000010"},
        {0x5327E2, 5,  "lea rdx, [rsp+0x28]"},
        {0x5327E7, 5,  "mov esi, 7"},
        {0x5327EC, 5,  "mov [rsp+0x28], rax"},
        {0x5327F1, 8,  "mov dword [rsp+0x30], 2"},
        {0x5327F9, 3,  "mov rdi, [rbx]"},
        {0x5327FC, 5,  "call 0xac5260"},
        // Vtable dispatch
        {0x532801, 3,  "mov rdi, [rbx]"},
        {0x532804, 3,  "mov rax, [rdi]"},
        {0x532807, 3,  "call [rax+0x48]"},
        {0x53280A, 3,  "mov rdi, [rbx]"},
        {0x53280D, 2,  "xor esi, esi"},
        {0x53280F, 3,  "mov rax, [rdi]"},
        {0x532812, 3,  "call [rax+0x28]"},
        {0x532815, 3,  "mov rdi, [rbx]"},
        {0x532818, 3,  "mov rax, [rdi]"},
        {0x53281B, 3,  "call [rax+0x50]"},
        {0x53281E, 3,  "mov rsi, [rbx]"},
        {0x532821, 8,  "lea rdi, [rsp+0x570]"},
        {0x532829, 2,  "xor edx, edx"},
        {0x53282B, 3,  "mov rax, [rsi]"},
        {0x53282E, 3,  "call [rax+0x20]"},
        // Float compare
        {0x532831, 9,  "vmovss xmm0, [rsp+0x58c]"},
        {0x53283A, 8,  "vucomiss xmm0, [0x1014c40]"},
        {0x532842, 6,  "jbe 0x5328cf"},
        // Container check
        {0x532848, 7,  "lea rcx, [0x2ea5170]"},
        {0x53284F, 3,  "mov rax, [rcx]"},
        {0x532852, 4,  "cmp rax, [rcx+8]"},
        {0x532856, 2,  "je 0x532864"},
        {0x532858, 7,  "mov rax, [rax+0x128]"},
        {0x53285F, 3,  "cmp byte [rax], 0"},
        {0x532862, 2,  "je 0x5328bc"},
        // Logging sub-block
        {0x532864, 7,  "lea rax, [0x1014db9]"},
        {0x53286B, 7,  "lea r9, [0x1014ded]"},
        {0x532872, 8,  "lea rdx, [rsp+0x450]"},
        {0x53287A, 5,  "mov edi, 0"},
        {0x53287F, 5,  "mov esi, 1"},
        {0x532884, 2,  "xor ecx, ecx"},
        {0x532886, 6,  "mov r8d, 1"},
        {0x53288C, 5,  "vmovq xmm0, rax"},
        {0x532891, 7,  "lea rax, [0x1014d71]"},
        {0x532898, 5,  "vmovq xmm1, rax"},
        {0x53289D, 2,  "xor eax, eax"},
        {0x53289F, 4,  "vpunpcklqdq xmm0, xmm1, xmm0"},
        {0x5328A3, 9,  "vmovdqa [rsp+0x450], xmm0"},
        {0x5328AC, 11, "mov dword [rsp+0x460], 0x202"},
        {0x5328B7, 5,  "call 0x3e7e0"},
        // Success cleanup
        {0x5328BC, 2,  "xor edi, edi"},
        {0x5328BE, 3,  "xor r12d, r12d"},
        {0x5328C1, 5,  "call 0x4c4950"},
        {0x5328C6, 4,  "mov r15, [r13+0]"},
        {0x5328CA, 3,  "xor r14d, r14d"},
        {0x5328CD, 2,  "jmp 0x532904"},
        // Error-catch path
        {0x5328CF, 5,  "mov edi, 1"},
        {0x5328D4, 5,  "call 0x4c4950"},
        {0x5328D9, 4,  "mov r15, [r13+0]"},
        {0x5328DD, 3,  "mov r12b, 1"},
        // 36 B preamble
        {0x5328E0, 7,  "mov rax, [0x2efe318]"},
        {0x5328E7, 7,  "lea rdi, [0x2efe318]"},
        {0x5328EE, 2,  "xor esi, esi"},
        {0x5328F0, 2,  "call [rax]"},
        {0x5328F2, 7,  "mov rax, [0x2efe340]"},
        {0x5328F9, 4,  "mov rax, [rax+0x60]"},
        {0x5328FD, 3,  "cmp byte [rax], 0"},
        {0x532900, 4,  "sete r14b"},
    }};

// Host stand-ins for the pinned operands.
struct Init531d60PostBaseAlt5327bdHooks {
    // Gate object [0x2dfce70] host stand-in.  The original does
    // `lea rax,[0x2dfce70]; mov rax,[rax+0x60]; cmp byte [rax],0`,
    // i.e. loads a pointer at +0x60 and reads the byte it points to.
    // The stand-in collapses the double indirection: gate_2dfce70
    // points at a byte buffer whose byte at +kGateOff is the gate flag.
    const void* gate_2dfce70 = nullptr;

    // Object cell [0x3305568] host stand-in for vtable-heavy dispatch.
    const void* obj_3305568 = nullptr;

    // Container object [0x2ea5170] host stand-in: head at +0, tail at +8.
    const void* container_2ea5170 = nullptr;

    // Preamble cells.
    const void* cell_2efe318 = nullptr;
    const void* cell_2efe340 = nullptr;

    // The error-path preamble's `call [rax]` at 0x5328f0 is slot 0 of
    // 0x2efe318.  It may use the same native lazy-bind model as continue-C.
    void (*vcall_slot0)(void* rdi, std::uint8_t esi) = nullptr;
    bool use_native_lazy_bind_91210 = false;
    void* demo_object = nullptr;
    LazyBind91210Hooks lazy_bind{};

    // Incoming r13 (parent/init pointer, provenance NOT modelled).
    const void* incoming_r13 = nullptr;

    // Callee hooks.  Null => no-op (unit still counted).
    void (*call_ac5260)(void* obj, void* rdx, int esi) = nullptr;
    void (*call_3e7e0)(int edi, int esi, int ecx, int r8d,
                       const void* rdx, const void* r9,
                       void* xmm_packed) = nullptr;
    void (*call_4c4950)(int edi) = nullptr;

    // Outcome overrides (for testing branch paths).
    // When gate_check_result is supplied, the cmp/jne gate is evaluated:
    //   gate_check_result != 0 => jne taken (goes to error path).
    std::uint8_t gate_check_result = 0;
    bool gate_check_supplied = false;

    // Float compare result override.
    //   float_below_equal == true => jbe taken (goes to error path).
    bool float_below_equal = false;
    bool float_compare_supplied = false;

    // Container list flag override at [rax+0x128].
    std::uint8_t container_flag = 0;
    bool container_flag_supplied = false;

    // When true, skip the logging sub-block (je 0x5328bc taken).
    bool skip_logging = false;
    bool skip_logging_supplied = false;
};

struct Init531d60PostBaseAlt5327bdResult {
    bool ok = false;

    // Per-unit execution counters.
    std::array<int, kInit531d60PostBaseAlt5327bdUnitCount> unit_counts{};
    int unit_executed = 0;       // expect 70

    // Category counters.
    int call_direct_count = 0;   // expect 4 (ac5260, 3e7e0, 4c4950×2)
    int vcall_count = 0;         // expect 5 (+0x48,+0x28,+0x50,+0x20,[rax])
    int lea_rip_count = 0;       // expect 7 (2dfce70, 3305568, 2ea5170, 1014db9,
                                 //         1014ded, 1014d71, 2efe318)
    int mov_rip_count = 0;       // expect 2 (mov rax,[0x2efe318], mov rax,[0x2efe340])
    int float_compare_count = 0; // expect 1 (vucomiss xmm0,[0x1014c40])
    int branch_count = 0;        // expect 5 (jne + jbe + je×2 + jmp)
    int setcc_count = 0;         // expect 1 (sete)
    int slot0_dispatch_count = 0; // actual error-path dispatches at 0x5328f0
    int lazy_bind_calls = 0;
    int lazy_bind_last_returned = 0;
    bool lazy_bind_ok = false;

    // Exit-stub state (modelled, not in byte span).
    bool exit_stub_active = false;  // true when the 0x5327b6 path was taken

    // Join behaviour.
    // reached_via_jmp:  true  → success path took `jmp 0x532904` (unit 58).
    //                   false → error path fell through preamble to `sete r14b`.
    bool reached_via_jmp = false;
    bool reached_via_preamble = false;

    // Flags at join.
    std::uint8_t  r12b_out = 0;
    std::uint8_t  r14b_out = 0;
    bool          r12b_known = false;
    bool          r14b_known = false;

    // Pinned end address.
    std::uint64_t end_rva = kInit531d60PostBaseAlt5327bdEndRva;
};

// Runs the OBS `0x5327bd..0x532904` unit sequence: the alternate init path
// entered when batch A's `je 0x5327bd` is taken.
[[nodiscard]] Init531d60PostBaseAlt5327bdResult init_531d60_post_base_alt_5327bd(
    const Init531d60PostBaseAlt5327bdHooks& hooks = {}) noexcept;

// Models the `0x5327b6` exit stub: sets eax=0 and returns to 0x532c6f.
// Call when the `je 0x5327b6` at 0x531de1 was taken.
inline void init_531d60_post_base_exit_5327b6(
    Init531d60PostBaseAlt5327bdResult& out) noexcept {
    out.exit_stub_active = true;
}

}  // namespace sotc
