// Init `0x531d60` continue call-site OBS `0x5325c6..0x5325cb` (5 B):
//   call 0xa2ed00   ; fourth unit after E497 second post-a2ed00-u3 0xf590
//   // stop before mov rdi,rbx / call 0xf590 @ 0x5325cb
//
// Always-on after E497 second post-a2ed00-u3 0xf590. Intervening prep
// `0x532565..0x5325c6` (97 B: mov rdi,[r13] + stack lea/mov/vmovups) is NOT
// modelled — prefer this call-site over stack-prep tourism and over
// `0xa2ed00` body. Prefer over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` /
// `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`. Prefer over
// trampoline/`0x52eb50` / `0x26800`/`0xf590` / staffroll / vcall-callee /
// `0xa2caf0` body tourism.
//
// Distinct from first `a2ed00` (E489), second `a2ed00_u2` (E492), and third
// `a2ed00_u3` (E495).
// Claimed: call-site control flow pin only.
// NOT modelled: 97 B stack prep; 0xa2ed00 body; following call 0xf590.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseA2ed00U4Rva = 0x5325c6;
inline constexpr std::size_t kInit531d60PostBaseA2ed00U4Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseA2ed00U4EndRva = 0x5325cb;
inline constexpr std::uint64_t kInit531d60PostBaseA2ed00U4TargetRva = 0xa2ed00;
inline constexpr std::uint64_t kInit531d60PostBaseA2ed00U4PrepRva = 0x532565;
inline constexpr std::size_t kInit531d60PostBaseA2ed00U4PrepSize = 97;

struct Init531d60PostBaseA2ed00U4Hooks {
    // Models call 0xa2ed00 (callee body NOT lifted). Null → no-op.
    void (*call_a2ed00)() = nullptr;
};

struct Init531d60PostBaseA2ed00U4Result {
    bool ok = false;
    int call_a2ed00_count = 0;
};

// Models OBS 0x5325c6..0x5325cb on measured Init success path.
[[nodiscard]] Init531d60PostBaseA2ed00U4Result init_531d60_post_base_a2ed00_u4(
    const Init531d60PostBaseA2ed00U4Hooks& hooks = {}) noexcept;

}  // namespace sotc
