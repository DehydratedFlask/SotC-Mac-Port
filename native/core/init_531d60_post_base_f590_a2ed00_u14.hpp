// Init `0x531d60` continue call-site OBS `0x532723..0x532728` (5 B):
//   call 0xf590   ; second unit after E508 first post-a2ed00-u7 0xf590 call-site
//   // stop before mov rbx,[r13] @ 0x532728
//
// Always-on after E508 first post-a2ed00-u7 0xf590. Intervening prep
// `0x532720..0x532723` (3 B: mov rdi,r15) is NOT modelled — prefer this
// call-site over mov-rdi prep tourism and over `0xf590` body. Prefer over
// deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
// `0x14c2f0` / `0x6a5370`. Prefer over trampoline/`0x52eb50` / `0x26800` /
// staffroll / vcall-callee / `0xa2ed00` / `0xa2caf0` body tourism.
//
// Distinct from earlier dual call-site `26800_f590`, `f590_a2ed00` (E490),
// `f590_a2ed00_u2` (E491), `f590_a2ed00_u3` (E493), `f590_a2ed00_u4`
// (E494), `f590_a2ed00_u5` (E496), `f590_a2ed00_u6` (E497),
// `f590_a2ed00_u7` (E499), `f590_a2ed00_u8` (E500), `f590_a2ed00_u9`
// (E502), `f590_a2ed00_u10` (E503), `f590_a2ed00_u11` (E505),
// `f590_a2ed00_u12` (E506), and `f590_a2ed00_u13` (E508).
// Claimed: call-site control flow pin only.
// NOT modelled: mov rdi,r15 prep; 0xf590 body; following mov rbx,[r13].
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "shared_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U14Rva = 0x532723;
inline constexpr std::size_t kInit531d60PostBaseF590A2ed00U14Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U14EndRva = 0x532728;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U14TargetRva = 0xf590;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U14PrepRva = 0x532720;
inline constexpr std::size_t kInit531d60PostBaseF590A2ed00U14PrepSize = 3;

struct Init531d60PostBaseF590A2ed00U14Hooks {
    // 0xf590(rdi = &holder) at 0x532723. Original: 32 B stack temp at rsp+0x58, `+0x10..0x1f`
    // zeroed before the call, so the release finds no control block and does nothing.
    // Null => run the lifted release; non-null => host override, whose result is reported instead.
    SharedReleaseF590Result (*call_f590)(SharedHolderF590&) = nullptr;
    // 0x12af0 destroy / 0x170b0 free-control contracts; only reached with a non-null `+0x18`.
    SharedReleaseF590Hooks release_hooks{};
};

struct Init531d60PostBaseF590A2ed00U14Result {
    bool ok = false;
    int call_f590_count = 0;
    // Measured 0xf590 effect: the holder's `+0x18` is zero, so nothing is destroyed or freed.
    SharedReleaseF590Result release{};
};

// Models OBS 0x532723..0x532728 on measured Init success path.
[[nodiscard]] Init531d60PostBaseF590A2ed00U14Result init_531d60_post_base_f590_a2ed00_u14(
    const Init531d60PostBaseF590A2ed00U14Hooks& hooks = {}) noexcept;

}  // namespace sotc
