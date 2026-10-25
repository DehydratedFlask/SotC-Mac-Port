// Init `0x531d60` continue call-site OBS `0x531e0d..0x531e12` (5 B):
//   call 0x5b3d00
//   // stop before next call 0x5b3cf0 @ 0x531e12
//
// Always-on after E450 0x56db70 call-site. Target 0x5b3d00 is a one-byte `ret`
// (same class as E406 0x56dca0 / E450 0x56db70 ret-nop) — claim the call-site,
// not a body. Prefer over deepen 0x1290 / 0x5c64c0 / 0x56dcb0 / 0x499f80 /
// 0x140d90 / 0x14c2f0 / 0x6a5370. Prefer over empty-path migrate/non-null.
//
// Claimed: call-site control flow pin only (+ target is-ret identity).
// NOT modelled: later Init continues (0x5b3cf0…). PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase5b3d00Rva = 0x531e0d;
inline constexpr std::size_t kInit531d60PostBase5b3d00Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase5b3d00EndRva = 0x531e12;
inline constexpr std::uint64_t kInit531d60PostBase5b3d00TargetRva = 0x5b3d00;

struct Init531d60PostBase5b3d00Hooks {
    // Models 0x5b3d00() (ret-nop). Null → no-op.
    void (*call_5b3d00)() = nullptr;
};

struct Init531d60PostBase5b3d00Result {
    bool ok = false;
    int call_5b3d00_count = 0;
};

// Models OBS 0x531e0d..0x531e12 on measured Init success path.
[[nodiscard]] Init531d60PostBase5b3d00Result init_531d60_post_base_5b3d00(
    const Init531d60PostBase5b3d00Hooks& hooks = {}) noexcept;

}  // namespace sotc
