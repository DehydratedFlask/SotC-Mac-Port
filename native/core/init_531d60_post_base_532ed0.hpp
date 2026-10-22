// Init `0x531d60` continue call-site OBS `0x5322f8..0x5322fd` (5 B):
//   call 0x532ed0   ; first unit after E475 fifth virtual-call +0x18
//   // stop before movq/virtual-call path @ 0x5322fd
//
// Always-on after E475 vcall18_u5. The five-unit 0x532ca0+vcall18 chain
// closed; this is the next always-on call (not another 0x532ca0).
// Intervening stack prep `0x532285..0x5322f8` (115 B) is NOT modelled —
// prefer this call-site over stack-prep tourism and over `0x532ed0` body
// (`push rbp` helper). Prefer over deepen `0x1290` / `0x5c64c0` /
// `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`. Prefer
// over trampoline/`0x52eb50` / `0x26800`/`0xf590` / staffroll /
// vcall-callee / `0x532ca0` body tourism.
//
// Claimed: call-site control flow pin only.
// NOT modelled: stack prep; 0x532ed0 body; following virtual call. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase532ed0Rva = 0x5322f8;
inline constexpr std::size_t kInit531d60PostBase532ed0Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase532ed0EndRva = 0x5322fd;
inline constexpr std::uint64_t kInit531d60PostBase532ed0TargetRva = 0x532ed0;
inline constexpr std::uint64_t kInit531d60PostBase532ed0StackPrepRva = 0x532285;
inline constexpr std::size_t kInit531d60PostBase532ed0StackPrepSize = 115;

struct Init531d60PostBase532ed0Hooks {
    // Models call 0x532ed0 (callee body NOT lifted). Null → no-op.
    void (*call_532ed0)() = nullptr;
};

struct Init531d60PostBase532ed0Result {
    bool ok = false;
    int call_532ed0_count = 0;
};

// Models OBS 0x5322f8..0x5322fd on measured Init success path.
[[nodiscard]] Init531d60PostBase532ed0Result init_531d60_post_base_532ed0(
    const Init531d60PostBase532ed0Hooks& hooks = {}) noexcept;

}  // namespace sotc
