// Init `0x531d60` continue call-site OBS `0x5326ba..0x5326bf` (5 B):
//   call 0xf590   ; first unit after E504 sixth 0xa2ed00 call-site
//   // stop before mov rdi,r15 / call 0xf590 @ 0x5326bf
//
// Always-on after E504 sixth 0xa2ed00. Intervening prep
// `0x5326b7..0x5326ba` (3 B: mov rdi,rbx) is NOT modelled — prefer this
// call-site over mov-rdi prep tourism and over `0xf590` body. Prefer over
// deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
// `0x14c2f0` / `0x6a5370`. Prefer over trampoline/`0x52eb50` / `0x26800` /
// staffroll / vcall-callee / `0xa2ed00` / `0xa2caf0` body tourism.
//
// Distinct from earlier dual call-site `26800_f590`, `f590_a2ed00` (E490),
// `f590_a2ed00_u2` (E491), `f590_a2ed00_u3` (E493), `f590_a2ed00_u4`
// (E494), `f590_a2ed00_u5` (E496), `f590_a2ed00_u6` (E497),
// `f590_a2ed00_u7` (E499), `f590_a2ed00_u8` (E500), `f590_a2ed00_u9`
// (E502), and `f590_a2ed00_u10` (E503).
// Claimed: call-site control flow pin only.
// NOT modelled: mov rdi,rbx prep; 0xf590 body; following call 0xf590.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U11Rva = 0x5326ba;
inline constexpr std::size_t kInit531d60PostBaseF590A2ed00U11Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U11EndRva = 0x5326bf;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U11TargetRva = 0xf590;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U11PrepRva = 0x5326b7;
inline constexpr std::size_t kInit531d60PostBaseF590A2ed00U11PrepSize = 3;

struct Init531d60PostBaseF590A2ed00U11Hooks {
    // Models call 0xf590 (callee body NOT lifted). Null → no-op.
    void (*call_f590)() = nullptr;
};

struct Init531d60PostBaseF590A2ed00U11Result {
    bool ok = false;
    int call_f590_count = 0;
};

// Models OBS 0x5326ba..0x5326bf on measured Init success path.
[[nodiscard]] Init531d60PostBaseF590A2ed00U11Result init_531d60_post_base_f590_a2ed00_u11(
    const Init531d60PostBaseF590A2ed00U11Hooks& hooks = {}) noexcept;

}  // namespace sotc
