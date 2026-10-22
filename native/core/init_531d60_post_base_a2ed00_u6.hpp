// Init `0x531d60` continue call-site OBS `0x5326b2..0x5326b7` (5 B):
//   call 0xa2ed00   ; sixth unit after E503 second post-a2ed00-u5 0xf590
//   // stop before mov rdi,rbx / call 0xf590 @ 0x5326b7
//
// Always-on after E503 second post-a2ed00-u5 0xf590. Intervening prep
// `0x532654..0x5326b2` (94 B: mov rdi,[r13] + stack lea/mov/vmovups) is NOT
// modelled — prefer this call-site over stack-prep tourism and over
// `0xa2ed00` body. Prefer over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` /
// `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`. Prefer over
// trampoline/`0x52eb50` / `0x26800`/`0xf590` / staffroll / vcall-callee /
// `0xa2caf0` body tourism.
//
// Distinct from first `a2ed00` (E489), second `a2ed00_u2` (E492), third
// `a2ed00_u3` (E495), fourth `a2ed00_u4` (E498), and fifth `a2ed00_u5`
// (E501).
// Claimed: call-site control flow pin only.
// NOT modelled: 94 B stack prep; 0xa2ed00 body; following call 0xf590.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseA2ed00U6Rva = 0x5326b2;
inline constexpr std::size_t kInit531d60PostBaseA2ed00U6Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseA2ed00U6EndRva = 0x5326b7;
inline constexpr std::uint64_t kInit531d60PostBaseA2ed00U6TargetRva = 0xa2ed00;
inline constexpr std::uint64_t kInit531d60PostBaseA2ed00U6PrepRva = 0x532654;
inline constexpr std::size_t kInit531d60PostBaseA2ed00U6PrepSize = 94;

struct Init531d60PostBaseA2ed00U6Hooks {
    // Models call 0xa2ed00 (callee body NOT lifted). Null → no-op.
    void (*call_a2ed00)() = nullptr;
};

struct Init531d60PostBaseA2ed00U6Result {
    bool ok = false;
    int call_a2ed00_count = 0;
};

// Models OBS 0x5326b2..0x5326b7 on measured Init success path.
[[nodiscard]] Init531d60PostBaseA2ed00U6Result init_531d60_post_base_a2ed00_u6(
    const Init531d60PostBaseA2ed00U6Hooks& hooks = {}) noexcept;

}  // namespace sotc
