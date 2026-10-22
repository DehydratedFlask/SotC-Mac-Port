// Init `0x531d60` continue call-site OBS `0x53245d..0x532462` (5 B):
//   call 0xa2ed00   ; first unit after E488 first 0xa2caf0 call-site
//   // stop before mov rdi,rbx / call 0xf590 @ 0x532462
//
// Always-on after E488 first 0xa2caf0. Intervening prep `0x5323f5..0x53245d`
// (104 B: second mov rdi,[r13] + stack lea/mov/vmovups) is NOT modelled —
// prefer this call-site over stack-prep tourism and over `0xa2ed00` body.
// Prefer over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` /
// `0x140d90` / `0x14c2f0` / `0x6a5370`. Prefer over trampoline/`0x52eb50` /
// `0x26800`/`0xf590` / staffroll / vcall-callee / `0xa2caf0` body tourism.
//
// Claimed: call-site control flow pin only.
// NOT modelled: 104 B stack prep; 0xa2ed00 body; following call 0xf590.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseA2ed00Rva = 0x53245d;
inline constexpr std::size_t kInit531d60PostBaseA2ed00Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseA2ed00EndRva = 0x532462;
inline constexpr std::uint64_t kInit531d60PostBaseA2ed00TargetRva = 0xa2ed00;
inline constexpr std::uint64_t kInit531d60PostBaseA2ed00PrepRva = 0x5323f5;
inline constexpr std::size_t kInit531d60PostBaseA2ed00PrepSize = 104;

struct Init531d60PostBaseA2ed00Hooks {
    // Models call 0xa2ed00 (callee body NOT lifted). Null → no-op.
    void (*call_a2ed00)() = nullptr;
};

struct Init531d60PostBaseA2ed00Result {
    bool ok = false;
    int call_a2ed00_count = 0;
};

// Models OBS 0x53245d..0x532462 on measured Init success path.
[[nodiscard]] Init531d60PostBaseA2ed00Result init_531d60_post_base_a2ed00(
    const Init531d60PostBaseA2ed00Hooks& hooks = {}) noexcept;

}  // namespace sotc
