// App vslot `0x533100` stack-check + success epilogue OBS `0x534052..0x534074` (34 B):
//   rax = *0x15fe518; rax = *rax; cmp [rbp-0x30]; jne fail
//   success: add rsp,0x2d8; pop callee-saves; pop rbp; ret
//   fail @ 0x534074: call 0xf8f3a0 (__stack_chk_fail) — hooked only, not body
//
// Always-on after E444 tail 0x140d90 call-site. Prefer over full 0x140d90 body
// (143 B) and over deepen 0x14c2f0 / 0x6a5370 (already hooked earlier).
// Prefer over empty-path migrate/non-null. Completes measured 0x533100 body.
//
// Claimed: canary cell pin + compare + success ret control flow.
// NOT modelled: body of 0xf8f3a0; body of 0x140d90; real frame layout;
// bodies of 0x14c2f0 / 0x6a5370. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100StackchkRva = 0x534052;
inline constexpr std::size_t kVslot40533100StackchkSize = 34;
inline constexpr std::uint64_t kVslot40StackchkEndRva = 0x534074;
inline constexpr std::uint64_t kVslot40StackchkGuardCellRva = 0x15fe518;
inline constexpr std::uint64_t kVslot40StackchkFailRva = 0xf8f3a0;

struct Vslot40_533100StackchkHooks {
    // Models **0x15fe518 live guard load. Null → use stack_guard_value_cell().
    std::uint64_t (*load_guard)() = nullptr;
    // Models call 0xf8f3a0 on mismatch. Null → no-op.
    void (*stack_chk_fail_f8f3a0)() = nullptr;
};

struct Vslot40_533100StackchkResult {
    bool ok = false;
    bool returned = false;
    bool took_fail_path = false;
    std::uint64_t saved = 0;
    std::uint64_t live = 0;
    int fail_count = 0;
};

// Process-wide guard word modelling the value behind *0x15fe518.
[[nodiscard]] std::uint64_t& stack_guard_value_cell() noexcept;

// Models stack-check through success ret. Pass saved == live for measured success.
[[nodiscard]] Vslot40_533100StackchkResult vslot_40_533100_stackchk(
    std::uint64_t saved_canary,
    const Vslot40_533100StackchkHooks& hooks = {}) noexcept;

}  // namespace sotc
