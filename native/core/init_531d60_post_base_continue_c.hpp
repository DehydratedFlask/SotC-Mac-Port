// Init `0x531d60` continue OBS `0x532904..0x53299d` (153 B, 38 consecutive units).
//
// This is the MERGE block reached unconditionally by the continue_b fall-through
// (`0x5327b1 e9 4e 01 00 00` = `jmp 0x532904`), and also by the continue_b je
// taken path (`0x5328e0`, a straight-line 36 B run that performs the same
// `call [rax]` vtable dispatch and stores `sete r14b`, then falls into 0x532904).
//
// Structure (three subsystem dispatches sharing one vtable source 0x2efe318):
//   0x532904  movzx esi,r14b              (4 B)   r14b = 0 on the fall-through
//                                                path; sete result on the taken path
//   0x532908  mov rdi,r15                 (3 B)
//   0x53290b  call 0xa52b70               (5 B)   <- direct rel32
//   0x532910  lea rax,[0x32f8b80]         (7 B)   <- RIP-rel lea (obj A cell)
//   0x532917  lea r15,[0x2efe318]         (7 B)   <- RIP-rel lea (vtable cell; r15 reused)
//   0x53291e  xor esi,esi                 (2 B)
//   0x532920  mov rdi,r15                 (3 B)
//   0x532923  mov rbx,[rax]               (3 B)   rbx = *obj_A
//   0x532926  mov rax,[0x2efe318]         (7 B)   <- RIP-rel global load (vtable)
//   0x53292d  call [rax]                  (2 B)   <- indirect vtable slot 0
//   0x53292f  mov rax,[0x2efe340]         (7 B)   <- RIP-rel global load (flag src)
//   0x532936  xor esi,esi                 (2 B)
//   0x532938  mov rdi,rbx                 (3 B)
//   0x53293b  mov rax,[rax+0x60]          (4 B)
//   0x53293f  cmp byte [rax],0            (3 B)
//   0x532942  sete sil                    (4 B)
//   0x532946  call 0xa599e0               (5 B)   <- direct rel32
//   0x53294b  lea rax,[0x32f6d18]         (7 B)   <- RIP-rel lea (obj B cell)
//   0x532952  xor esi,esi                 (2 B)
//   0x532954  mov rdi,r15                 (3 B)
//   0x532957  mov rbx,[rax]               (3 B)   rbx = *obj_B
//   0x53295a  mov rax,[0x2efe318]         (7 B)   <- RIP-rel global load (vtable)
//   0x532961  call [rax]                  (2 B)   <- indirect vtable slot 0
//   0x532963  mov rax,[0x2efe340]         (7 B)   <- RIP-rel global load (flag src)
//   0x53296a  xor esi,esi                 (2 B)
//   0x53296c  mov rdi,rbx                 (3 B)
//   0x53296f  mov rax,[rax+0x60]          (4 B)
//   0x532973  cmp byte [rax],0            (3 B)
//   0x532976  sete sil                    (4 B)
//   0x53297a  call 0xa431e0               (5 B)   <- direct rel32
//   0x53297f  mov rax,[0x2efe318]         (7 B)   <- RIP-rel global load (vtable)
//   0x532986  xor esi,esi                 (2 B)
//   0x532988  mov rdi,r15                 (3 B)
//   0x53298b  call [rax]                  (2 B)   <- indirect vtable slot 0
//   0x53298d  mov rax,[0x2efe340]         (7 B)   <- RIP-rel global load (flag src)
//   0x532994  mov rax,[rax+0x60]          (4 B)
//   0x532998  cmp byte [rax],0            (3 B)
//   0x53299b  je 0x5329ae                 (2 B)   <- structural boundary (outcome NOT modelled)
//
// One contiguous run from the merge entry up to the FIRST conditional branch,
// modelled as ONE grouped module.
//
// Claimed (observations, each byte span pinned by
// `tools/lift/check_f531d60_post_base_continue_c.py`):
//   * exact 153 B span `0x532904..0x53299d` and its 38 instructions;
//   * the three direct-call rel32 targets 0xa52b70 / 0xa599e0 / 0xa431e0;
//   * the three indirect `call [rax]` sites (vtable slot 0, rax = [0x2efe318]);
//   * the three RIP-relative lea targets 0x32f8b80 / 0x2efe318 / 0x32f6d18;
//   * the six RIP-relative global loads [0x2efe318] x3 and [0x2efe340] x3;
//   * the je rel8 target 0x5329ae;
//   * ordering: call 0xa52b70 first (esi = r14b, rdi = incoming r15), then three
//     vtable slot-0 dispatches interleaved with two direct calls, each direct call
//     consuming rdi = the object loaded from the matching lea cell and
//     esi = (byte at [0x2efe340]->[+0x60]->byte == 0).
//
// NOT modelled (do not read as semantics):
//   * bodies of 0xa52b70 / 0xa599e0 / 0xa431e0. The vtable slot-0 callee stays a
//     counting hook unless `use_native_lazy_bind_91210` is set, in which case
//     `run_lazy_bind_91210` binds `demo_object+0x28` and the flag byte is
//     `*(*(demo_object+0x28)+0x60)`;
//   * BOTH successors (`0x53299d` fall-through block, whose head is
//     `lea rax,[0x32f79a8]`, and `0x5329ae` branch target). The `je` outcome is
//     modelled only when merge_flag_byte is supplied (taken when byte is 0).
//     Execution of batch D/E is composed in `appmain_init_ctor_98a90_slice` (E535/E536),
//     not inside this module's runner.
//   * the incoming r15 and r14b provenance (r14b is 0 on the fall-through path
//     and the `sete r14b` result on the `0x5328e0` taken path — both upstream);
//   * the pointer values held in globals 0x32f8b80 / 0x32f6d18 / 0x2efe318 /
//     0x2efe340, and the identity of the resolved vtable slot-0 function;
//   * any effect on Init's observable result, GL/audio state, or save data.
//
// Distinct from E509 (continue_a) and E510 (continue_b) and all earlier modules.
// G1 remains PARTIAL.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "lazy_bind_91210.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseContinueCRva = 0x532904;
inline constexpr std::size_t kInit531d60PostBaseContinueCSize = 153;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCEndRva = 0x53299d;
// First conditional branch of the merge run: pinned target, outcome NOT modelled.
inline constexpr std::uint64_t kInit531d60PostBaseContinueCBranchRva = 0x53299b;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCBranchTargetRva = 0x5329ae;
// Direct-call rel32 targets.
inline constexpr std::uint64_t kInit531d60PostBaseContinueCCallA52b70Rva = 0xa52b70;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCCallA599e0Rva = 0xa599e0;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCCallA431e0Rva = 0xa431e0;
// RIP-relative lea targets.
inline constexpr std::uint64_t kInit531d60PostBaseContinueCLea32f8b80Rva = 0x32f8b80;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCLea2efe318Rva = 0x2efe318;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCLea32f6d18Rva = 0x32f6d18;
// RIP-relative global load cells (mov rax,[rip+…]).
inline constexpr std::uint64_t kInit531d60PostBaseContinueCGlobal2efe318Rva = 0x2efe318;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCGlobal2efe340Rva = 0x2efe340;

// Unit identity for the 38 pinned instructions of the claimed run.
enum class ContinueCUnit : int {
    movzx_esi_r14b = 0,     // 0x532904
    mov_rdi_r15,            // 0x532908
    call_a52b70,            // 0x53290b
    lea_rax_32f8b80,        // 0x532910
    lea_r15_2efe318,        // 0x532917
    xor_esi_esi_1,          // 0x53291e
    mov_rdi_r15_1,          // 0x532920
    mov_rbx_rax_1,          // 0x532923
    mov_rax_2efe318_1,      // 0x532926
    vcall_slot0_1,          // 0x53292d
    mov_rax_2efe340_1,      // 0x53292f
    xor_esi_esi_2,          // 0x532936
    mov_rdi_rbx_1,          // 0x532938
    mov_rax_rax60_1,        // 0x53293b
    cmp_byte_rax0_1,        // 0x53293f
    sete_sil_1,             // 0x532942
    call_a599e0,            // 0x532946
    lea_rax_32f6d18,        // 0x53294b
    xor_esi_esi_3,          // 0x532952
    mov_rdi_r15_2,          // 0x532954
    mov_rbx_rax_2,          // 0x532957
    mov_rax_2efe318_2,      // 0x53295a
    vcall_slot0_2,          // 0x532961
    mov_rax_2efe340_2,      // 0x532963
    xor_esi_esi_4,          // 0x53296a
    mov_rdi_rbx_2,          // 0x53296c
    mov_rax_rax60_2,        // 0x53296f
    cmp_byte_rax0_2,        // 0x532973
    sete_sil_2,             // 0x532976
    call_a431e0,            // 0x53297a
    mov_rax_2efe318_3,      // 0x53297f
    xor_esi_esi_5,          // 0x532986
    mov_rdi_r15_3,          // 0x532988
    vcall_slot0_3,          // 0x53298b
    mov_rax_2efe340_3,      // 0x53298d
    mov_rax_rax60_3,        // 0x532994
    cmp_byte_rax0_3,        // 0x532998
    je_5329ae,              // 0x53299b (boundary; outcome NOT modelled)
    count
};

inline constexpr int kInit531d60PostBaseContinueCUnitCount =
    static_cast<int>(ContinueCUnit::count);

struct ContinueCUnitSpan {
    std::uint64_t rva = 0;
    std::uint8_t size = 0;
    const char* mnemonic = "";
};

// OBS table re-derived from local/re/eboot.elf; sizes sum to kSize (153 B).
inline constexpr std::array<ContinueCUnitSpan, kInit531d60PostBaseContinueCUnitCount>
    kInit531d60PostBaseContinueCUnits{{
        {0x532904, 4, "movzx esi,r14b"},
        {0x532908, 3, "mov rdi,r15"},
        {0x53290b, 5, "call 0xa52b70"},
        {0x532910, 7, "lea rax,[0x32f8b80]"},
        {0x532917, 7, "lea r15,[0x2efe318]"},
        {0x53291e, 2, "xor esi,esi"},
        {0x532920, 3, "mov rdi,r15"},
        {0x532923, 3, "mov rbx,[rax]"},
        {0x532926, 7, "mov rax,[0x2efe318]"},
        {0x53292d, 2, "call [rax]"},
        {0x53292f, 7, "mov rax,[0x2efe340]"},
        {0x532936, 2, "xor esi,esi"},
        {0x532938, 3, "mov rdi,rbx"},
        {0x53293b, 4, "mov rax,[rax+0x60]"},
        {0x53293f, 3, "cmp byte [rax],0"},
        {0x532942, 4, "sete sil"},
        {0x532946, 5, "call 0xa599e0"},
        {0x53294b, 7, "lea rax,[0x32f6d18]"},
        {0x532952, 2, "xor esi,esi"},
        {0x532954, 3, "mov rdi,r15"},
        {0x532957, 3, "mov rbx,[rax]"},
        {0x53295a, 7, "mov rax,[0x2efe318]"},
        {0x532961, 2, "call [rax]"},
        {0x532963, 7, "mov rax,[0x2efe340]"},
        {0x53296a, 2, "xor esi,esi"},
        {0x53296c, 3, "mov rdi,rbx"},
        {0x53296f, 4, "mov rax,[rax+0x60]"},
        {0x532973, 3, "cmp byte [rax],0"},
        {0x532976, 4, "sete sil"},
        {0x53297a, 5, "call 0xa431e0"},
        {0x53297f, 7, "mov rax,[0x2efe318]"},
        {0x532986, 2, "xor esi,esi"},
        {0x532988, 3, "mov rdi,r15"},
        {0x53298b, 2, "call [rax]"},
        {0x53298d, 7, "mov rax,[0x2efe340]"},
        {0x532994, 4, "mov rax,[rax+0x60]"},
        {0x532998, 3, "cmp byte [rax],0"},
        {0x53299b, 2, "je 0x5329ae"},
    }};

// Host stand-ins for the pinned operands. Every field is optional: a null cell
// only skips that host-side read (the unit is still counted as executed), and a
// null call hook is a no-op. Nothing here reconstructs callee behaviour.
struct Init531d60PostBaseContinueCHooks {
    // Incoming registers at the merge entry (provenance upstream, NOT modelled):
    // r14b is 0 on the continue_b fall-through path and the `sete r14b` result on
    // the `0x5328e0` taken path; r15 is a stable init pointer from far upstream.
    std::uint8_t incoming_r14b = 0;   // movzx esi,r14b -> esi for call 0xa52b70
    // E528/E529: when the batch-B `je 0x5328e0` is taken, the 36 B preamble
    // 0x5328e0..0x532904 runs first and its `sete r14b` becomes incoming_r14b.
    // The caller sets this flag; the preamble requires vcall_slot0 and
    // preamble_flag_byte (resolved [0x2efe340]+0x60 byte), else it fails visibly.
    bool enter_via_5328e0 = false;
    const std::uint8_t* preamble_flag_byte = nullptr;
    const void* incoming_r15 = nullptr;  // mov rdi,r15 -> rdi for call 0xa52b70

    // RIP-relative global cells (addresses pinned; the pointer values they hold
    // are host stand-ins, dereferenced only to route the calls below):
    const void* const* cell_32f8b80 = nullptr;  // lea @0x532910; rbx = *cell (obj A)
    const void* const* cell_32f6d18 = nullptr;  // lea @0x53294b; rbx = *cell (obj B)
    const void* const* cell_2efe318 = nullptr;  // vtable src (mov @0x532926/0x53295a/0x53297f)
    const void* const* cell_2efe340 = nullptr;  // flag src (mov @0x53292f/0x532963/0x53298d)

    // Resolved `cmp byte [rax],0` operands for the two sete instructions (the
    // actual byte is [0x2efe340]->[+0x60]->byte; the host supplies the resolved
    // location). null => flag unknown, sil place-holder 0, still counted.
    const std::uint8_t* flag_byte_iter1 = nullptr;  // 0x53293f; sil -> call 0xa599e0
    const std::uint8_t* flag_byte_iter2 = nullptr;  // 0x532973; sil -> call 0xa431e0
    // iter3 flag (0x532998) feeds the merge `je 0x5329ae`: the branch is taken when the
    // resolved byte is 0. Null => outcome NOT modelled.

    // Direct-call hooks (callee bodies NOT lifted; null => no-op):
    void (*call_a52b70)(void* rdi, std::uint8_t esi) = nullptr;
    void (*call_a599e0)(void* rdi, std::uint8_t esi) = nullptr;
    void (*call_a431e0)(void* rdi, std::uint8_t esi) = nullptr;
    // Indirect vtable-slot-0 dispatch (call [rax], rax = [0x2efe318]); null => no-op.
    void (*vcall_slot0)(void* rdi, std::uint8_t esi) = nullptr;
    // Resolved [0x2efe340]+0x60 byte for the merge `je 0x5329ae` (0x532998).
    const std::uint8_t* merge_flag_byte = nullptr;

    // When set, the slot-0 sites call run_lazy_bind_91210(demo_object).
    // demo_object is the 0x2efe318 object (bound pointer at +0x28).
    // A resolved `*(*(object+0x28)+0x60)` fills any null flag-byte slot.
    bool use_native_lazy_bind_91210 = false;
    void* demo_object = nullptr;
    LazyBind91210Hooks lazy_bind{};
};

struct Init531d60PostBaseContinueCResult {
    bool ok = false;
    // Per-unit execution counters (all must be 1 on the modelled path).
    std::array<int, kInit531d60PostBaseContinueCUnitCount> unit_counts{};
    int unit_executed = 0;         // expect 38
    int call_count = 0;            // expect 3 direct rel32 call sites
    int vcall_count = 0;           // expect 3 indirect `call [rax]` slot-0 sites
    int lea_count = 0;             // expect 3 RIP-relative lea
    int global_mov_count = 0;      // expect 6 RIP-relative global loads
    int xor_count = 0;             // expect 5 (xor esi,esi)
    int cmp_count = 0;             // expect 3 (cmp byte [rax],0)
    int sete_count = 0;            // expect 2 (sete sil)
    int branch_boundary_count = 0; // expect 1 (je pinned, outcome NOT modelled)
    // sete sil results (host stand-in; 0/unknown when the flag byte hook is null).
    bool flag1_known = false;
    bool flag2_known = false;
    std::uint8_t sil_iter1 = 0;
    std::uint8_t sil_iter2 = 0;
    // Pinned next addresses (constants, not measured control flow).
    std::uint64_t end_rva = kInit531d60PostBaseContinueCEndRva;
    std::uint64_t branch_target_rva = kInit531d60PostBaseContinueCBranchTargetRva;
    bool branch_outcome_modelled = false;
    // Merge `je 0x5329ae` (0x53299b): modelled only when merge_flag_byte is supplied.
    bool merge_branch_outcome_modelled = false;
    bool merge_branch_taken = false;
    // 0x5328e0..0x532904 taken-path preamble (8 units, 36 B), only when
    // enter_via_5328e0. Its sete r14b feeds the merge entry's movzx esi,r14b.
    bool preamble_ran = false;
    int preamble_unit_executed = 0;   // expect 8
    int preamble_call_count = 0;      // expect 1 (call [rax] slot 0)
    bool preamble_flag_known = false;
    std::uint8_t preamble_r14b = 0;
    // Effective value consumed by 0x532904 movzx esi,r14b (preamble result when
    // the taken path ran, else incoming_r14b).
    std::uint8_t first_esi = 0;
    // Native 0x91210 invocations (preamble plus the three merge-block sites).
    int lazy_bind_calls = 0;
    int lazy_bind_last_returned = 0;
    bool lazy_bind_ok = false;
};

// Runs the OBS `0x532904..0x53299d` unit sequence on the measured Init path.
[[nodiscard]] Init531d60PostBaseContinueCResult init_531d60_post_base_continue_c(
    const Init531d60PostBaseContinueCHooks& hooks = {}) noexcept;

}  // namespace sotc
