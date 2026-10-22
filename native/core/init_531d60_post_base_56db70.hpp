// Init `0x531d60` continue call-site OBS `0x531e08..0x531e0d` (5 B):
//   call 0x56db70
//   // stop before next call 0x5b3d00 @ 0x531e0d
//
// Always-on after E449 0x1290 call-site. Target 0x56db70 is a one-byte `ret`
// (same class as E406 0x56dca0 ret-nop) — claim the call-site, not a body.
// Prefer over deepen 0x1290 / 0x5c64c0 / 0x56dcb0 / 0x499f80 / 0x140d90 /
// 0x14c2f0 / 0x6a5370. Prefer over empty-path migrate/non-null.
//
// Claimed: call-site control flow pin only (+ target is-ret identity).
// NOT modelled: later Init continues (0x5b3d00…). PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase56db70Rva = 0x531e08;
inline constexpr std::size_t kInit531d60PostBase56db70Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase56db70EndRva = 0x531e0d;
inline constexpr std::uint64_t kInit531d60PostBase56db70TargetRva = 0x56db70;

struct Init531d60PostBase56db70Hooks {
    // Models 0x56db70() (ret-nop). Null → no-op.
    void (*call_56db70)() = nullptr;
};

struct Init531d60PostBase56db70Result {
    bool ok = false;
    int call_56db70_count = 0;
};

// Models OBS 0x531e08..0x531e0d on measured Init success path.
[[nodiscard]] Init531d60PostBase56db70Result init_531d60_post_base_56db70(
    const Init531d60PostBase56db70Hooks& hooks = {}) noexcept;

}  // namespace sotc
