// Init `0x531d60` continue OBS `0x532740..0x53277f` (63 B, 13 consecutive units):
//   0x532740  lea rax,[rip+0x2dc5261]      (7 B)  -> 0x32f79a8
//   0x532747  lea r13,[rip+0x2dc5daa]      (7 B)  -> 0x32f84f8
//   0x53274e  mov rax,[rax]                (3 B)
//   0x532751  mov rdi,[r13]                (4 B)
//   0x532755  mov rsi,[rax+0xf8]           (7 B)
//   0x53275c  add rsi,0x40                 (4 B)
//   0x532760  call 0xa543b0                (5 B)
//   0x532765  mov rax,[rsp+0x20]           (5 B)
//   0x53276a  lea rsi,[rip+0xae25f8]       (7 B)  -> 0x1014d69
//   0x532771  lea rdi,[rax+8]              (4 B)
//   0x532775  call 0x1fc80                 (5 B)
//   0x53277a  test rax,rax                 (3 B)
//   0x53277d  je 0x5327bd                  (2 B)  <- structural boundary
//
// Always-on after E511 first `0xa2e5d0` call-site (0x53273b..0x532740).
// This is one contiguous branch-free run up to the FIRST conditional branch after
// the tip, so it is modelled as ONE grouped module instead of 13 five-byte modules.
//
// Claimed (observations, each byte span pinned by
// `tools/lift/check_f531d60_post_base_continue_a.py`):
//   * exact 63 B span `0x532740..0x53277f` and its 13 instructions;
//   * the two direct-call rel32 targets 0xa543b0 / 0x1fc80; the four RIP-relative
//     lea targets 0x32f79a8 / 0x32f84f8 / 0x1014d69 and the rax+8 form; the `je`
//     rel8 target 0x5327bd;
//   * ordering: the two call sites are sequenced in this exact order, and the
//     last call consumes rdi/rsi built by 0x532740..0x532771 (as observed).
//
// NOT modelled (do not read as semantics):
//   * bodies of 0xa543b0 — hooks are host stand-ins, null hook => no-op.
//     0x1fc80 stays a counting hook unless `use_native_lookup_1fc80` is set,
//     in which case `lookup_1fc80` supplies rax;
//   * BOTH successors (`0x53277f` fall-through block and `0x5327bd` branch target).
//     The `je` outcome is modelled only when rax from 0x1fc80 is known
//     (taken when that pointer is null);
//   * provenance of the incoming r13 (set earlier in Init) and of r12/r14/r15;
//   * object layouts at 0x32f79a8 / 0x32f84f8 beyond the observed `+0xf8` read,
//     the contents of the data at 0x1014d69 (used only as an RVA pin), and the
//     real stack frame (only the `0x20(%rsp)` slot is exposed as a host cell);
//   * any effect on Init's observable result, GL/audio state, or save data.
//
// Distinct from E489..E511 (single call-site chain) and from the earlier
// `lea_vmovq` (E465) lea-cell module.
// G1 remains PARTIAL. PR #5 cite-only.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "init_531d60_post_base_continue_b.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseContinueARva = 0x532740;
inline constexpr std::size_t kInit531d60PostBaseContinueASize = 63;
inline constexpr std::uint64_t kInit531d60PostBaseContinueAEndRva = 0x53277f;
// First conditional branch after the tip: pinned target, outcome NOT modelled.
inline constexpr std::uint64_t kInit531d60PostBaseContinueABranchRva = 0x53277d;
inline constexpr std::uint64_t kInit531d60PostBaseContinueABranchTargetRva = 0x5327bd;
inline constexpr std::uint64_t kInit531d60PostBaseContinueACallA543b0Rva = 0xa543b0;
inline constexpr std::uint64_t kInit531d60PostBaseContinueACall1fc80Rva = 0x1fc80;
inline constexpr std::uint64_t kInit531d60PostBaseContinueALea32f79a8Rva = 0x32f79a8;
inline constexpr std::uint64_t kInit531d60PostBaseContinueALea32f84f8Rva = 0x32f84f8;
inline constexpr std::uint64_t kInit531d60PostBaseContinueALea1014d69Rva = 0x1014d69;
// Byte offset read at 0x532755 (`mov rsi,[rax+0xf8]`).
inline constexpr std::size_t kInit531d60PostBaseContinueAObjOff = 0xf8;
// `add rsi,0x40` at 0x53275c.
inline constexpr std::size_t kInit531d60PostBaseContinueARsiAddend = 0x40;

// Unit identity for the 13 pinned instructions of the claimed run.
enum class ContinueAUnit : int {
    lea_rax_32f79a8,    // 0x532740
    lea_r13_32f84f8,    // 0x532747
    load_rax_rax,       // 0x53274e
    load_rdi_r13,       // 0x532751
    load_rsi_rax_f8,    // 0x532755
    add_rsi_40,         // 0x53275c
    call_a543b0,        // 0x532760
    load_rax_rsp20,     // 0x532765
    lea_rsi_1014d69,    // 0x53276a
    lea_rdi_rax8,       // 0x532771
    call_1fc80,         // 0x532775
    test_rax_rax,       // 0x53277a
    je_5327bd,          // 0x53277d (boundary; outcome NOT modelled)
    count
};

inline constexpr int kInit531d60PostBaseContinueAUnitCount =
    static_cast<int>(ContinueAUnit::count);

struct ContinueAUnitSpan {
    std::uint64_t rva = 0;
    std::uint8_t size = 0;
    const char* mnemonic = "";
};

// OBS table re-derived from local/re/eboot.elf; sizes sum to kSize (63 B).
inline constexpr std::array<ContinueAUnitSpan, kInit531d60PostBaseContinueAUnitCount>
    kInit531d60PostBaseContinueAUnits{{
        {0x532740, 7, "lea rax,[0x32f79a8]"},
        {0x532747, 7, "lea r13,[0x32f84f8]"},
        {0x53274e, 3, "mov rax,[rax]"},
        {0x532751, 4, "mov rdi,[r13]"},
        {0x532755, 7, "mov rsi,[rax+0xf8]"},
        {0x53275c, 4, "add rsi,0x40"},
        {0x532760, 5, "call 0xa543b0"},
        {0x532765, 5, "mov rax,[rsp+0x20]"},
        {0x53276a, 7, "lea rsi,[0x1014d69]"},
        {0x532771, 4, "lea rdi,[rax+8]"},
        {0x532775, 5, "call 0x1fc80"},
        {0x53277a, 3, "test rax,rax"},
        {0x53277d, 2, "je 0x5327bd"},
    }};

// Host stand-ins for the pinned operands. Every field is optional: a null cell
// only skips that host-side read (the unit is still counted as executed), and a
// null call hook is a no-op. Nothing here reconstructs callee behaviour.
struct Init531d60PostBaseContinueAHooks {
    // Global cell 0x32f79a8 (lea at 0x532740 / deref at 0x53274e). When set it
    // must point at a host stand-in object of at least kObjOff + 8 == 0x100 B,
    // because 0x532755 reads 8 B at +0xf8 through it.
    const void* const* cell_32f79a8 = nullptr;
    // Global cell 0x32f84f8 (lea at 0x532747 / deref at 0x532751).
    const void* const* cell_32f84f8 = nullptr;
    // 0x20(%rsp) saved slot read at 0x532765.
    const void* const* rsp20_slot = nullptr;

    // Callee bodies NOT lifted. Null => no-op, counter still advances.
    // 0x532760.
    void (*call_a543b0)(void* rdi, std::uintptr_t rsi) = nullptr;
    // 0x532775; rsi_rva is the pinned data RVA 0x1014d69 (not a host pointer).
    // Counting hook: does not supply rax. Null => no-op.
    void (*call_1fc80)(void* rdi, std::uintptr_t rsi_rva) = nullptr;
    // When non-null, its return is rax and the je outcome is modelled.
    // Overrides use_native_lookup_1fc80.
    void* (*call_1fc80_result)(void* rdi, std::uintptr_t rsi_rva) = nullptr;
    // When true (and call_1fc80_result is null), run lookup_1fc80 on rdi with
    // nointro_needle. The needle is the host C string for data RVA 0x1014d69.
    bool use_native_lookup_1fc80 = false;
    const char* nointro_needle = nullptr;
};

struct Init531d60PostBaseContinueAResult {
    bool ok = false;
    // Per-unit execution counters (all must be 1 on the modelled path).
    std::array<int, kInit531d60PostBaseContinueAUnitCount> unit_counts{};
    int unit_executed = 0;       // expect 13
    int call_count = 0;          // expect 2 call sites sequenced
    int lea_count = 0;           // expect 4
    int load_count = 0;          // host-side reads performed (expect 4 when cells set)
    int alu_count = 0;           // expect 2 (add rsi,0x40 / test rax,rax)
    int branch_boundary_count = 0;  // expect 1 (je pinned)
    // Observed plumbing at 0x53275c: ([*0x32f79a8 + 0xf8] + 0x40) when cells set.
    std::uintptr_t rsi_at_0x532760 = 0;
    bool rsi_known = false;
    // Pinned next addresses (constants, not measured control flow).
    std::uint64_t end_rva = kInit531d60PostBaseContinueAEndRva;
    std::uint64_t branch_target_rva = kInit531d60PostBaseContinueABranchTargetRva;
    bool branch_outcome_modelled = false;
    // Set only when call_1fc80_result or use_native_lookup_1fc80 produced rax.
    void* rax_from_1fc80 = nullptr;
    bool rax_from_1fc80_known = false;
    // je 0x5327bd is taken when the known rax is null.
    bool branch_taken = false;
};

struct InitContinueAbNativeResult {
    Init531d60PostBaseContinueAResult cont_a{};
    Init531d60PostBaseContinueBResult cont_b{};
    bool ran_continue_b = false;
};

// continue_a with native 0x1fc80, then continue_b with config_bool_lookup_init_continue_b
// when the lookup hit (rax non-null). Miss leaves continue_b unrun.
[[nodiscard]] InitContinueAbNativeResult run_init_continue_ab_native_lookups(
    Init531d60PostBaseContinueAHooks a_hooks,
    Init531d60PostBaseContinueBHooks b_hooks = {}) noexcept;

// Runs the OBS `0x532740..0x53277f` unit sequence on the measured Init path.
[[nodiscard]] Init531d60PostBaseContinueAResult init_531d60_post_base_continue_a(
    const Init531d60PostBaseContinueAHooks& hooks = {}) noexcept;

}  // namespace sotc
