// Init `0x531d60` continue call-site OBS `0x532387..0x53238c` (5 B):
//   call 0x544fe0   ; first unit after E480 first 0x17070 call-site
//   // stop before lea r14 @ 0x53238c
//
// Always-on after E480 0x17070. Intervening prep `0x532381..0x532387`
// (6 B: mov rbx,rax; mov rdi,rbx) is NOT modelled — prefer this call-site
// over mov-rbx/arg-prep tourism and over `0x544fe0` body (`push rbp`
// helper). Prefer over deepen `0x1290` / `0x5c64c0` / `0x56dcb0` /
// `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`. Prefer over
// trampoline/`0x52eb50` / `0x26800`/`0xf590` / staffroll / vcall-callee /
// `0x532ed0` / `0x532ca0` / `0x17070` body tourism.
//
// Claimed: call-site control flow pin only.
// NOT modelled: mov rbx,rax / mov rdi,rbx prep; 0x544fe0 body; following
// cell-install (claimed separately as E482) / second 0x17070. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase544fe0Rva = 0x532387;
inline constexpr std::size_t kInit531d60PostBase544fe0Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase544fe0EndRva = 0x53238c;
inline constexpr std::uint64_t kInit531d60PostBase544fe0TargetRva = 0x544fe0;
inline constexpr std::uint64_t kInit531d60PostBase544fe0PrepRva = 0x532381;
inline constexpr std::size_t kInit531d60PostBase544fe0PrepSize = 6;

struct Init531d60PostBase544fe0Hooks {
    // Models call 0x544fe0 (callee body NOT lifted). Null → no-op.
    void (*call_544fe0)() = nullptr;
};

struct Init531d60PostBase544fe0Result {
    bool ok = false;
    int call_544fe0_count = 0;
};

// Models OBS 0x532387..0x53238c on measured Init success path.
[[nodiscard]] Init531d60PostBase544fe0Result init_531d60_post_base_544fe0(
    const Init531d60PostBase544fe0Hooks& hooks = {}) noexcept;

}  // namespace sotc
