// Init `0x531d60` continue call-site OBS `0x531e12..0x531e17` (5 B):
//   call 0x5b3cf0
//   // stop before next lea rax,[0x01b1dd40] @ 0x531e17
//
// Always-on after E451 0x5b3d00 call-site. Target 0x5b3cf0 is a one-byte `ret`
// (same class as E406/E450/E451 ret-nops) — claim the call-site, not a body.
// Prefer over deepen 0x1290 / 0x5c64c0 / 0x56dcb0 / 0x499f80 / 0x140d90 /
// 0x14c2f0 / 0x6a5370. Prefer over empty-path migrate/non-null.
//
// Claimed: call-site control flow pin only (+ target is-ret identity).
// NOT modelled: following AVX BSS copy / later calls (0x7f7220…). PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase5b3cf0Rva = 0x531e12;
inline constexpr std::size_t kInit531d60PostBase5b3cf0Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase5b3cf0EndRva = 0x531e17;
inline constexpr std::uint64_t kInit531d60PostBase5b3cf0TargetRva = 0x5b3cf0;

struct Init531d60PostBase5b3cf0Hooks {
    // Models 0x5b3cf0() (ret-nop). Null → no-op.
    void (*call_5b3cf0)() = nullptr;
};

struct Init531d60PostBase5b3cf0Result {
    bool ok = false;
    int call_5b3cf0_count = 0;
};

// Models OBS 0x531e12..0x531e17 on measured Init success path.
[[nodiscard]] Init531d60PostBase5b3cf0Result init_531d60_post_base_5b3cf0(
    const Init531d60PostBase5b3cf0Hooks& hooks = {}) noexcept;

}  // namespace sotc
