// Init `0x531d60` continue call-site OBS `0x53237c..0x532381` (5 B):
//   call 0x17070   ; first unit after E479 second 0x532ed0-series vcall18
//   // stop before mov rbx,rax @ 0x532381
//
// Always-on after E479 vcall18_532ed0_u2. The two-unit 0x532ed0+vcall18
// series closed; this is the next always-on call (not another 0x532ed0).
// Intervening arg prep `0x532377..0x53237c` (5 B: mov edi,0x460) is NOT
// modelled — prefer this call-site over arg-prep tourism and over
// `0x17070` body. Prefer over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` /
// `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`. Prefer over
// trampoline/`0x52eb50` / `0x26800`/`0xf590` / staffroll / vcall-callee /
// `0x532ed0` / `0x532ca0` body tourism.
//
// Claimed: call-site control flow pin only.
// NOT modelled: arg prep; 0x17070 body; following mov rbx,rax prep / 0x544fe0
// (claimed separately as E481).
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase17070Rva = 0x53237c;
inline constexpr std::size_t kInit531d60PostBase17070Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase17070EndRva = 0x532381;
inline constexpr std::uint64_t kInit531d60PostBase17070TargetRva = 0x17070;
inline constexpr std::uint64_t kInit531d60PostBase17070ArgPrepRva = 0x532377;
inline constexpr std::size_t kInit531d60PostBase17070ArgPrepSize = 5;
inline constexpr std::uint32_t kInit531d60PostBase17070ArgEdi = 0x460;

struct Init531d60PostBase17070Hooks {
    // Models call 0x17070 (callee body NOT lifted). Null → no-op.
    void (*call_17070)() = nullptr;
};

struct Init531d60PostBase17070Result {
    bool ok = false;
    int call_17070_count = 0;
};

// Models OBS 0x53237c..0x532381 on measured Init success path.
[[nodiscard]] Init531d60PostBase17070Result init_531d60_post_base_17070(
    const Init531d60PostBase17070Hooks& hooks = {}) noexcept;

}  // namespace sotc
