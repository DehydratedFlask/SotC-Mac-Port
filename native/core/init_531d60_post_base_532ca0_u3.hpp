// Init `0x531d60` continue call-site OBS `0x53218f..0x532194` (5 B):
//   call 0x532ca0   ; third unit after E469 second virtual-call +0x18
//   // stop before movq/virtual-call path @ 0x532194
//
// Always-on after E469 vcall18_u2. Intervening stack prep
// `0x532132..0x53218f` (93 B) is NOT modelled — prefer this call-site over
// stack-prep tourism and over `0x532ca0` body (`push rbp` helper). Prefer over
// deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
// `0x14c2f0` / `0x6a5370`. Prefer over trampoline/`0x52eb50` / `0x26800`/
// `0xf590` / staffroll / vcall-callee body tourism.
//
// Claimed: third-unit call-site control flow pin only.
// NOT modelled: stack prep; 0x532ca0 body; following virtual call
// (claimed separately as E471). PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase532ca0U3Rva = 0x53218f;
inline constexpr std::size_t kInit531d60PostBase532ca0U3Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase532ca0U3EndRva = 0x532194;
inline constexpr std::uint64_t kInit531d60PostBase532ca0U3TargetRva = 0x532ca0;
inline constexpr std::uint64_t kInit531d60PostBase532ca0U3StackPrepRva = 0x532132;
inline constexpr std::size_t kInit531d60PostBase532ca0U3StackPrepSize = 93;

struct Init531d60PostBase532ca0U3Hooks {
    // Models 0x532ca0() (helper body NOT lifted). Null → no-op.
    void (*call_532ca0)() = nullptr;
};

struct Init531d60PostBase532ca0U3Result {
    bool ok = false;
    int call_532ca0_count = 0;
};

// Models OBS 0x53218f..0x532194 on measured Init success path.
[[nodiscard]] Init531d60PostBase532ca0U3Result init_531d60_post_base_532ca0_u3(
    const Init531d60PostBase532ca0U3Hooks& hooks = {}) noexcept;

}  // namespace sotc
