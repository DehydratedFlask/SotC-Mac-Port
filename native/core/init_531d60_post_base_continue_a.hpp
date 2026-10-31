// Init `0x531d60` continue OBS `0x532765..0x53277f` (26 B, 6 consecutive units):
//   0x532765  mov rax,[rsp+0x20]           (5 B)
//   0x53276a  lea rsi,[rip+0xae25f8]       (7 B)  -> 0x1014d69
//   0x532771  lea rdi,[rax+8]              (4 B)
//   0x532775  call 0x1fc80                 (5 B)
//   0x53277a  test rax,rax                 (3 B)
//   0x53277d  je 0x5327bd                  (2 B)  <- structural boundary
//
// Always-on after E512 first `0xa543b0` call-site (0x532760..0x532765).
// This is one contiguous branch-free run up to the FIRST conditional branch after
// the tip, so it is modelled as ONE grouped module instead of six five-byte modules.
//
// Claimed (observations, each byte span pinned by
// `tools/lift/check_f531d60_post_base_continue_a.py`):
//   * exact 26 B span `0x532765..0x53277f` and its 6 instructions;
//   * the direct-call rel32 target 0x1fc80; the lea RIP-relative target 0x1014d69
//     and the rax+8 form; the `je` rel8 target 0x5327bd;
//   * ordering: 0x1fc80 runs after rdi/rsi are built by 0x532765..0x532771.
//
// NOT modelled (do not read as semantics):
//   * body of 0x1fc80 — counting hook or `lookup_1fc80` when enabled;
//   * BOTH successors (`0x53277f` fall-through block and `0x5327bd` branch target).
//     The `je` outcome is modelled only when rax from 0x1fc80 is known
//     (taken when that pointer is null);
//   * real stack frame beyond the `0x20(%rsp)` slot exposed as a host cell;
//   * contents of the data at 0x1014d69 (used only as an RVA pin);
//   * any effect on Init's observable result, GL/audio state, or save data.
//
// Distinct from E489..E512 (single call-site chain).
// G1 remains PARTIAL. PR #5 cite-only.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "init_531d60_post_base_continue_b.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseContinueARva = 0x532765;
inline constexpr std::size_t kInit531d60PostBaseContinueASize = 26;
inline constexpr std::uint64_t kInit531d60PostBaseContinueAEndRva = 0x53277f;
inline constexpr std::uint64_t kInit531d60PostBaseContinueABranchRva = 0x53277d;
inline constexpr std::uint64_t kInit531d60PostBaseContinueABranchTargetRva = 0x5327bd;
inline constexpr std::uint64_t kInit531d60PostBaseContinueACall1fc80Rva = 0x1fc80;
inline constexpr std::uint64_t kInit531d60PostBaseContinueALea1014d69Rva = 0x1014d69;

enum class ContinueAUnit : int {
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

inline constexpr std::array<ContinueAUnitSpan, kInit531d60PostBaseContinueAUnitCount>
    kInit531d60PostBaseContinueAUnits{{
        {0x532765, 5, "mov rax,[rsp+0x20]"},
        {0x53276a, 7, "lea rsi,[0x1014d69]"},
        {0x532771, 4, "lea rdi,[rax+8]"},
        {0x532775, 5, "call 0x1fc80"},
        {0x53277a, 3, "test rax,rax"},
        {0x53277d, 2, "je 0x5327bd"},
    }};

struct Init531d60PostBaseContinueAHooks {
    const void* const* rsp20_slot = nullptr;

    void (*call_1fc80)(void* rdi, std::uintptr_t rsi_rva) = nullptr;
    void* (*call_1fc80_result)(void* rdi, std::uintptr_t rsi_rva) = nullptr;
    bool use_native_lookup_1fc80 = false;
    const char* nointro_needle = nullptr;
};

struct Init531d60PostBaseContinueAResult {
    bool ok = false;
    std::array<int, kInit531d60PostBaseContinueAUnitCount> unit_counts{};
    int unit_executed = 0;       // expect 6
    int call_count = 0;          // expect 1
    int lea_count = 0;           // expect 2
    int load_count = 0;          // host-side reads performed (expect 1 when rsp set)
    int alu_count = 0;           // expect 1 (test rax,rax)
    int branch_boundary_count = 0;  // expect 1 (je pinned)
    std::uint64_t end_rva = kInit531d60PostBaseContinueAEndRva;
    std::uint64_t branch_target_rva = kInit531d60PostBaseContinueABranchTargetRva;
    bool branch_outcome_modelled = false;
    void* rax_from_1fc80 = nullptr;
    bool rax_from_1fc80_known = false;
    bool branch_taken = false;
};

struct InitContinueAbNativeResult {
    Init531d60PostBaseContinueAResult cont_a{};
    Init531d60PostBaseContinueBResult cont_b{};
    bool ran_continue_b = false;
};

[[nodiscard]] InitContinueAbNativeResult run_init_continue_ab_native_lookups(
    Init531d60PostBaseContinueAHooks a_hooks,
    Init531d60PostBaseContinueBHooks b_hooks = {}) noexcept;

[[nodiscard]] Init531d60PostBaseContinueAResult init_531d60_post_base_continue_a(
    const Init531d60PostBaseContinueAHooks& hooks = {}) noexcept;

}  // namespace sotc
