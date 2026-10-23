// Init `0x531d60` continue call-site OBS `0x532558..0x53255d` (5 B):
//   call 0xf590   ; first unit after E495 third 0xa2ed00 call-site
//   // stop before mov rdi,r15 / call 0xf590 @ 0x53255d
//
// Always-on after E495 third 0xa2ed00. Intervening prep `0x532555..0x532558`
// (3 B: mov rdi,rbx) is NOT modelled — prefer this call-site over mov-rdi
// prep tourism and over `0xf590` body. Prefer over deepen `0x1290` /
// `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
// `0x6a5370`. Prefer over trampoline/`0x52eb50` / `0x26800` / staffroll /
// vcall-callee / `0xa2ed00` / `0xa2caf0` body tourism.
//
// Distinct from earlier dual call-site `26800_f590`, `f590_a2ed00` (E490),
// `f590_a2ed00_u2` (E491), `f590_a2ed00_u3` (E493), and `f590_a2ed00_u4`
// (E494).
// Claimed: call-site control flow pin only.
// NOT modelled: mov rdi,rbx prep; 0xf590 body; following second call 0xf590.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "shared_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U5Rva = 0x532558;
inline constexpr std::size_t kInit531d60PostBaseF590A2ed00U5Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U5EndRva = 0x53255d;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U5TargetRva = 0xf590;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U5PrepRva = 0x532555;
inline constexpr std::size_t kInit531d60PostBaseF590A2ed00U5PrepSize = 3;

struct Init531d60PostBaseF590A2ed00U5Hooks {
    // 0xf590(rdi = &holder) at 0x532558. Original: 32 B stack temp at rsp+0x138, `+0x10..0x1f`
    // zeroed before the call, so the release finds no control block and does nothing.
    // Null => run the lifted release; non-null => host override, whose result is reported instead.
    SharedReleaseF590Result (*call_f590)(SharedHolderF590&) = nullptr;
    // 0x12af0 destroy / 0x170b0 free-control contracts; only reached with a non-null `+0x18`.
    SharedReleaseF590Hooks release_hooks{};
};

struct Init531d60PostBaseF590A2ed00U5Result {
    bool ok = false;
    int call_f590_count = 0;
    // Measured 0xf590 effect: the holder's `+0x18` is zero, so nothing is destroyed or freed.
    SharedReleaseF590Result release{};
};

// Models OBS 0x532558..0x53255d on measured Init success path.
[[nodiscard]] Init531d60PostBaseF590A2ed00U5Result init_531d60_post_base_f590_a2ed00_u5(
    const Init531d60PostBaseF590A2ed00U5Hooks& hooks = {}) noexcept;

}  // namespace sotc
