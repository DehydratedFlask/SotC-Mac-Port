// Init `0x531d60` continue call-site OBS `0x5323b9..0x5323be` (5 B):
//   call 0x53fd10   ; first unit after E483 second 0x17070 call-site
//   // stop before lea r14 @ 0x5323be
//
// Always-on after E483 second 0x17070. Intervening prep `0x5323b3..0x5323b9`
// (6 B: mov rbx,rax; mov rdi,rbx) is NOT modelled — prefer this call-site
// over mov-rbx/arg-prep tourism and over `0x53fd10` body (`push rbp`
// helper). Prefer over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` /
// `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`. Prefer over
// trampoline/`0x52eb50` / `0x26800`/`0xf590` / staffroll / vcall-callee /
// `0x544fe0` / `0x532ed0` / `0x532ca0` / `0x17070` body tourism.
//
// Claimed: call-site control flow pin only.
// NOT modelled: mov rbx,rax / mov rdi,rbx prep; 0x53fd10 body; following
// cell-install 0x2eff448 (claimed separately as E485). PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase53fd10Rva = 0x5323b9;
inline constexpr std::size_t kInit531d60PostBase53fd10Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase53fd10EndRva = 0x5323be;
inline constexpr std::uint64_t kInit531d60PostBase53fd10TargetRva = 0x53fd10;
inline constexpr std::uint64_t kInit531d60PostBase53fd10PrepRva = 0x5323b3;
inline constexpr std::size_t kInit531d60PostBase53fd10PrepSize = 6;

struct Init531d60PostBase53fd10Hooks {
    // Models call 0x53fd10 (callee body NOT lifted). Null → no-op.
    void (*call_53fd10)() = nullptr;
};

struct Init531d60PostBase53fd10Result {
    bool ok = false;
    int call_53fd10_count = 0;
};

// Models OBS 0x5323b9..0x5323be on measured Init success path.
[[nodiscard]] Init531d60PostBase53fd10Result init_531d60_post_base_53fd10(
    const Init531d60PostBase53fd10Hooks& hooks = {}) noexcept;

}  // namespace sotc
