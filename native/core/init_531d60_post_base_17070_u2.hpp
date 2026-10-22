// Init `0x531d60` continue call-site OBS `0x5323ae..0x5323b3` (5 B):
//   call 0x17070   ; second unit after E482 cell-install 0x2eff450
//   // stop before mov rbx,rax @ 0x5323b3
//
// Always-on after E482 cell-install. Intervening arg prep
// `0x5323a9..0x5323ae` (5 B: mov edi,0x490) is NOT modelled — prefer this
// call-site over arg-prep tourism and over `0x17070` body. Prefer over
// deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
// `0x14c2f0` / `0x6a5370`. Prefer over trampoline/`0x52eb50` /
// `0x26800`/`0xf590` / staffroll / vcall-callee / `0x544fe0` / `0x532ed0` /
// `0x532ca0` body tourism.
//
// Claimed: call-site control flow pin only.
// NOT modelled: arg prep; 0x17070 body; following mov rbx,rax / 0x53fd10.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase17070U2Rva = 0x5323ae;
inline constexpr std::size_t kInit531d60PostBase17070U2Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase17070U2EndRva = 0x5323b3;
inline constexpr std::uint64_t kInit531d60PostBase17070U2TargetRva = 0x17070;
inline constexpr std::uint64_t kInit531d60PostBase17070U2ArgPrepRva = 0x5323a9;
inline constexpr std::size_t kInit531d60PostBase17070U2ArgPrepSize = 5;
inline constexpr std::uint32_t kInit531d60PostBase17070U2ArgEdi = 0x490;

struct Init531d60PostBase17070U2Hooks {
    // Models call 0x17070 (callee body NOT lifted). Null → no-op.
    void (*call_17070)() = nullptr;
};

struct Init531d60PostBase17070U2Result {
    bool ok = false;
    int call_17070_count = 0;
};

// Models OBS 0x5323ae..0x5323b3 on measured Init success path.
[[nodiscard]] Init531d60PostBase17070U2Result init_531d60_post_base_17070_u2(
    const Init531d60PostBase17070U2Hooks& hooks = {}) noexcept;

}  // namespace sotc
