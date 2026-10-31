// Init `0x531d60` continue OBS `0x53277a..0x53277f` (5 B, 2 consecutive units):
//   0x53277a  test rax,rax                 (3 B)
//   0x53277d  je 0x5327bd                  (2 B)  <- structural boundary
//
// Always-on after E513 first `0x1fc80` call-site (0x532775..0x53277a).
// Grouped tail through the first conditional branch after the Init continue tip.
//
// Claimed: exact 5 B span and je rel8 target 0x5327bd; je outcome modelled only
// when incoming rax from E513 u1 (or explicit hook) is known.
// NOT modelled: 0x53277f fall-through and 0x5327bd successor blocks.
// G1 remains PARTIAL. PR #5 cite-only.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "init_531d60_post_base_continue_b.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseContinueARva = 0x53277a;
inline constexpr std::size_t kInit531d60PostBaseContinueASize = 5;
inline constexpr std::uint64_t kInit531d60PostBaseContinueAEndRva = 0x53277f;
inline constexpr std::uint64_t kInit531d60PostBaseContinueABranchRva = 0x53277d;
inline constexpr std::uint64_t kInit531d60PostBaseContinueABranchTargetRva = 0x5327bd;

enum class ContinueAUnit : int {
    test_rax_rax,       // 0x53277a
    je_5327bd,          // 0x53277d (boundary; outcome NOT modelled unless rax known)
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
        {0x53277a, 3, "test rax,rax"},
        {0x53277d, 2, "je 0x5327bd"},
    }};

struct Init531d60PostBaseContinueAHooks {
    // rax stand-in after E513 u1 (or explicit host injection).
    void* incoming_rax_from_1fc80 = nullptr;
    bool incoming_rax_from_1fc80_known = false;

    // Consumed only by run_init_continue_ab_native_lookups (delegates to 1fc80_u1).
    const void* const* rsp20_slot = nullptr;
    void (*call_1fc80)(void* rdi, std::uintptr_t rsi_rva) = nullptr;
    void* (*call_1fc80_result)(void* rdi, std::uintptr_t rsi_rva) = nullptr;
    bool use_native_lookup_1fc80 = false;
    const char* nointro_needle = nullptr;
};

struct Init531d60PostBaseContinueAResult {
    bool ok = false;
    std::array<int, kInit531d60PostBaseContinueAUnitCount> unit_counts{};
    int unit_executed = 0;       // expect 2
    int call_count = 0;          // expect 0 (call demoted to E513 u1)
    int lea_count = 0;           // expect 0
    int load_count = 0;          // expect 0
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
