// Init `0x531d60` continue call-site OBS `0x53246d..0x532472` (5 B):
//   call 0xf590   ; second unit after E490 first post-a2ed00 0xf590
//   // stop before mov rdi,[r13] @ 0x532472
//
// Always-on after E490 first post-a2ed00 0xf590. Intervening prep
// `0x53246a..0x53246d` (3 B: mov rdi,r15) is NOT modelled — prefer this
// call-site over mov-rdi prep tourism and over `0xf590` body. Prefer over
// deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
// `0x14c2f0` / `0x6a5370`. Prefer over trampoline/`0x52eb50` / `0x26800` /
// staffroll / vcall-callee / `0xa2ed00` / `0xa2caf0` body tourism.
//
// Distinct from earlier dual call-site `26800_f590` and first post-a2ed00
// `f590_a2ed00` (E490).
// Claimed: call-site control flow pin only.
// NOT modelled: mov rdi,r15 prep; 0xf590 body; following mov rdi,[r13].
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "shared_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U2Rva = 0x53246d;
inline constexpr std::size_t kInit531d60PostBaseF590A2ed00U2Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U2EndRva = 0x532472;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U2TargetRva = 0xf590;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U2PrepRva = 0x53246a;
inline constexpr std::size_t kInit531d60PostBaseF590A2ed00U2PrepSize = 3;

struct Init531d60PostBaseF590A2ed00U2Hooks {
    // 0xf590(rdi = &holder) at 0x53246d. Original: 32 B stack temp at rsp+0x1d8, `+0x10..0x1f`
    // zeroed before the call, so the release finds no control block and does nothing.
    // Null => run the lifted release; non-null => host override, whose result is reported instead.
    SharedReleaseF590Result (*call_f590)(SharedHolderF590&) = nullptr;
    // 0x12af0 destroy / 0x170b0 free-control contracts; only reached with a non-null `+0x18`.
    SharedReleaseF590Hooks release_hooks{};
};

struct Init531d60PostBaseF590A2ed00U2Result {
    bool ok = false;
    int call_f590_count = 0;
    // Measured 0xf590 effect: the holder's `+0x18` is zero, so nothing is destroyed or freed.
    SharedReleaseF590Result release{};
};

// Models OBS 0x53246d..0x532472 on measured Init success path.
[[nodiscard]] Init531d60PostBaseF590A2ed00U2Result init_531d60_post_base_f590_a2ed00_u2(
    const Init531d60PostBaseF590A2ed00U2Hooks& hooks = {}) noexcept;

}  // namespace sotc
