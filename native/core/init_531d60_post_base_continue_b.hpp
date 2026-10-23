// Init `0x531d60` continue OBS `0x53277f..0x5327b1` (50 B, 14 consecutive units):
//   0x53277f  xor esi,esi                 (2 B)
//   0x532781  xor edx,edx                 (2 B)
//   0x532783  mov ecx,1                   (5 B)
//   0x532788  mov rdi,rax                 (3 B)
//   0x53278b  xor r14d,r14d               (3 B)
//   0x53278e  call 0x1e4c0                (5 B)
//   0x532793  mov ebx,eax                 (2 B)
//   0x532795  mov r12b,bl                 (3 B)
//   0x532798  xor r12b,1                  (4 B)
//   0x53279c  movzx edi,r12b              (4 B)
//   0x5327a0  call 0x4c4950               (5 B)
//   0x5327a5  mov rdi,[r13+0]             (4 B)
//   0x5327a9  test bl,bl                  (2 B)
//   0x5327ab  je 0x5328e0                 (6 B)  <- structural boundary
//
// Fall-through block of the continue_a `je 0x5327bd` (je NOT taken, rax != 0).
// This is one contiguous run from the fall-through entry up to the NEXT
// conditional branch, so it is modelled as ONE grouped module.
//
// Claimed (observations, each byte span pinned by
// `tools/lift/check_f531d60_post_base_continue_b.py`):
//   * exact 50 B span `0x53277f..0x5327b1` and its 14 instructions;
//   * the two direct-call rel32 targets 0x1e4c0 / 0x4c4950;
//   * the je rel32 target 0x5328e0;
//   * ordering: the two call sites are sequenced in this exact order,
//     and the second call consumes edi built from r12b (xor with 1 of bl,
//     where bl is the low byte of the 0x1e4c0 return eax);
//   * the r13-based load at 0x5327a5 reads from [0x32f84f8] (cell set by
//     continue_a at 0x532747, NOT re-pinned in this module — provenance
//     is upstream).
//
// Branch outcome (E524): `0x1e4c0` is a config-value lookup that returns a
// boolean on every path (`movb $1` / `xor` / `setne` -> eax), so when the call
// hook supplies a value in {0,1} the `test bl,bl` / `je 0x5328e0` outcome is
// modelled: bl == 0 => taken, bl != 0 => not taken. Any other value, or no
// hook, leaves `branch_outcome_modelled == false`.
//
// NOT modelled (do not read as semantics):
//   * bodies of 0x1e4c0 and 0x4c4950 — hooks are host stand-ins,
//     null hook => no-op;
//   * BOTH successors (`0x5327b1` fall-through block and the 36 B
//     `0x5328e0` preamble that also falls into the 0x532904 merge);
//   * the actual non-zero rax value (return from 0x1fc80, NOT modelled);
//   * provenance of rax, r13, and the flag computed by continue_a
//     (r13 = [0x32f84f8] from 0x532747);
//   * any effect on Init's observable result, GL/audio state, or save data.
//
// Distinct from E509 (continue_a) and from all earlier modules.
// G1 remains PARTIAL.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseContinueBRva = 0x53277f;
inline constexpr std::size_t kInit531d60PostBaseContinueBSize = 50;
inline constexpr std::uint64_t kInit531d60PostBaseContinueBEndRva = 0x5327b1;
// Next conditional branch after the continue_a je fall-through. Target is
// pinned; the outcome is modelled only when the 0x1e4c0 hook returns 0 or 1.
inline constexpr std::uint64_t kInit531d60PostBaseContinueBBranchRva = 0x5327ab;
inline constexpr std::uint64_t kInit531d60PostBaseContinueBBranchTargetRva = 0x5328e0;
inline constexpr std::uint64_t kInit531d60PostBaseContinueBCall1e4c0Rva = 0x1e4c0;
inline constexpr std::uint64_t kInit531d60PostBaseContinueBCall4c4950Rva = 0x4c4950;
// r13 cell loaded at 0x5327a5 (`mov rdi,[r13+0]`). r13 was set by continue_a
// at 0x532747 to 0x32f84f8; the same cell pointer is reused here.
inline constexpr std::uint64_t kInit531d60PostBaseContinueBR13CellRva = 0x32f84f8;

// Unit identity for the 14 pinned instructions of the claimed run.
enum class ContinueBUnit : int {
    xor_esi_esi = 0,    // 0x53277f
    xor_edx_edx,        // 0x532781
    mov_ecx_1,          // 0x532783
    mov_rdi_rax,        // 0x532788
    xor_r14d_r14d,      // 0x53278b
    call_1e4c0,         // 0x53278e
    mov_ebx_eax,        // 0x532793
    mov_r12b_bl,        // 0x532795
    xor_r12b_1,         // 0x532798
    movzx_edi_r12b,     // 0x53279c
    call_4c4950,        // 0x5327a0
    mov_rdi_r13_load,   // 0x5327a5
    test_bl_bl,         // 0x5327a9
    je_5328e0,          // 0x5327ab (boundary; outcome NOT modelled)
    count
};

inline constexpr int kInit531d60PostBaseContinueBUnitCount =
    static_cast<int>(ContinueBUnit::count);

struct ContinueBUnitSpan {
    std::uint64_t rva = 0;
    std::uint8_t size = 0;
    const char* mnemonic = "";
};

// OBS table re-derived from local/re/eboot.elf; sizes sum to kSize (50 B).
inline constexpr std::array<ContinueBUnitSpan, kInit531d60PostBaseContinueBUnitCount>
    kInit531d60PostBaseContinueBUnits{{
        {0x53277f, 2, "xor esi,esi"},
        {0x532781, 2, "xor edx,edx"},
        {0x532783, 5, "mov ecx,1"},
        {0x532788, 3, "mov rdi,rax"},
        {0x53278b, 3, "xor r14d,r14d"},
        {0x53278e, 5, "call 0x1e4c0"},
        {0x532793, 2, "mov ebx,eax"},
        {0x532795, 3, "mov r12b,bl"},
        {0x532798, 4, "xor r12b,1"},
        {0x53279c, 4, "movzx edi,r12b"},
        {0x5327a0, 5, "call 0x4c4950"},
        {0x5327a5, 4, "mov rdi,[r13+0]"},
        {0x5327a9, 2, "test bl,bl"},
        {0x5327ab, 6, "je 0x5328e0"},
    }};

// Host stand-ins for the pinned operands. Every field is optional: a null cell
// only skips that host-side read (the unit is still counted as executed), and a
// null call hook is a no-op. Nothing here reconstructs callee behaviour.
struct Init531d60PostBaseContinueBHooks {
    // rax at 0x532788 (`mov rdi,rax`): the non-zero return value from the
    // continue_a `call 0x1fc80`. When set, supplies the rax stand-in; when null
    // the unit is still counted but rdi is zeroed.
    const void* incoming_rax_standin = nullptr;
    // [0x32f84f8] cell read at 0x5327a5 (`mov rdi,[r13+0]`). r13 provenance
    // is upstream (continue_a 0x532747), NOT modelled here.
    const void* const* cell_32f84f8 = nullptr;

    // Callee bodies NOT lifted. Null => no-op, counter still advances.
    // 0x53278e; edi is the rax stand-in (or nullptr if incoming_rax_standin is null).
    int (*call_1e4c0)(void* rdi) = nullptr;
    // 0x5327a0; edi carries (r12b xor 1) where r12b = low byte of eax from call_1e4c0.
    void (*call_4c4950)(std::uint8_t edi) = nullptr;
};

struct Init531d60PostBaseContinueBResult {
    bool ok = false;
    // Per-unit execution counters (all must be 1 on the modelled path).
    std::array<int, kInit531d60PostBaseContinueBUnitCount> unit_counts{};
    int unit_executed = 0;         // expect 14
    int call_count = 0;            // expect 2 call sites sequenced
    int alu_count = 0;             // expect 5 (xor esi,esi / xor edx,edx /
                                   //         xor r14d,r14d / xor r12b,1 /
                                   //         test bl,bl)
    int load_count = 0;            // expect 1 ([r13+0])
    int branch_boundary_count = 0; // expect 1 (je pinned, outcome NOT modelled)
    // Observed eax from the 0x1e4c0 call hook (host stand-in; 0 if no hook).
    int eax_from_1e4c0 = 0;
    bool eax_known = false;
    // Pinned next addresses (constants, not measured control flow).
    std::uint64_t end_rva = kInit531d60PostBaseContinueBEndRva;
    std::uint64_t branch_target_rva = kInit531d60PostBaseContinueBBranchTargetRva;
    bool branch_outcome_modelled = false;
    // E524 rule, only meaningful when branch_outcome_modelled: je is taken
    // when bl == 0, because 0x1e4c0 returns a boolean.
    std::uint8_t bl = 0;
    bool branch_taken = false;
};

// Runs the OBS `0x53277f..0x5327b1` unit sequence on the measured Init path.
[[nodiscard]] Init531d60PostBaseContinueBResult init_531d60_post_base_continue_b(
    const Init531d60PostBaseContinueBHooks& hooks = {}) noexcept;

}  // namespace sotc
