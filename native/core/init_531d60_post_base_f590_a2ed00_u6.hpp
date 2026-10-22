// Init `0x531d60` continue call-site OBS `0x532560..0x532565` (5 B):
//   call 0xf590   ; second unit after E495 third 0xa2ed00 call-site
//   // stop before mov rdi,[r13] / stack prep for next 0xa2ed00 @ 0x532565
//
// Always-on after E496 first post-a2ed00-u3 0xf590. Intervening prep
// `0x53255d..0x532560` (3 B: mov rdi,r15) is NOT modelled — prefer this
// call-site over mov-rdi prep tourism and over `0xf590` body. Prefer over
// deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
// `0x14c2f0` / `0x6a5370`. Prefer over trampoline/`0x52eb50` / `0x26800` /
// staffroll / vcall-callee / `0xa2ed00` / `0xa2caf0` body tourism.
//
// Distinct from earlier dual call-site `26800_f590`, `f590_a2ed00` (E490),
// `f590_a2ed00_u2` (E491), `f590_a2ed00_u3` (E493), `f590_a2ed00_u4`
// (E494), and `f590_a2ed00_u5` (E496).
// Claimed: call-site control flow pin only.
// NOT modelled: mov rdi,r15 prep; 0xf590 body; following stack prep /
// fourth 0xa2ed00 call-site.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U6Rva = 0x532560;
inline constexpr std::size_t kInit531d60PostBaseF590A2ed00U6Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U6EndRva = 0x532565;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U6TargetRva = 0xf590;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U6PrepRva = 0x53255d;
inline constexpr std::size_t kInit531d60PostBaseF590A2ed00U6PrepSize = 3;

struct Init531d60PostBaseF590A2ed00U6Hooks {
    // Models call 0xf590 (callee body NOT lifted). Null → no-op.
    void (*call_f590)() = nullptr;
};

struct Init531d60PostBaseF590A2ed00U6Result {
    bool ok = false;
    int call_f590_count = 0;
};

// Models OBS 0x532560..0x532565 on measured Init success path.
[[nodiscard]] Init531d60PostBaseF590A2ed00U6Result init_531d60_post_base_f590_a2ed00_u6(
    const Init531d60PostBaseF590A2ed00U6Hooks& hooks = {}) noexcept;

}  // namespace sotc
