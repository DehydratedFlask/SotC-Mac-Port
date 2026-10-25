// Init `0x531d60` continue call-site OBS `0x531e41..0x531e46` (5 B):
//   call 0x56fad0
//   // stop before next call 0xf90310 @ 0x531e46
//
// Always-on after E454 0x7f7220 call-site. Target 0x56fad0 is a one-byte `ret`
// (same class as E406/E450–E452 ret-nops) — claim the call-site, not a body.
// Prefer over staffroll body 0x7f7220 and over deepen 0x1290 / 0x5c64c0 /
// 0x56dcb0 / 0x499f80 / 0x140d90 / 0x14c2f0 / 0x6a5370.
// Prefer over empty-path migrate/non-null.
//
// Claimed: call-site control flow pin only (+ target is-ret identity).
// NOT modelled: later Init continues (0xf90310…). PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase56fad0Rva = 0x531e41;
inline constexpr std::size_t kInit531d60PostBase56fad0Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase56fad0EndRva = 0x531e46;
inline constexpr std::uint64_t kInit531d60PostBase56fad0TargetRva = 0x56fad0;

struct Init531d60PostBase56fad0Hooks {
    // Models 0x56fad0() (ret-nop). Null → no-op.
    void (*call_56fad0)() = nullptr;
};

struct Init531d60PostBase56fad0Result {
    bool ok = false;
    int call_56fad0_count = 0;
};

// Models OBS 0x531e41..0x531e46 on measured Init success path.
[[nodiscard]] Init531d60PostBase56fad0Result init_531d60_post_base_56fad0(
    const Init531d60PostBase56fad0Hooks& hooks = {}) noexcept;

}  // namespace sotc
