// Init `0x531d60` continue call-site OBS `0x5323f0..0x5323f5` (5 B):
//   call 0xa2caf0   ; first unit after E487 lea r13 pin 0x32f4fe0
//   // stop before mov rdi,[r13] @ 0x5323f5
//
// Always-on after E487 lea r13. Intervening prep `0x5323ec..0x5323f0`
// (4 B: mov rdi,[r13]) is NOT modelled — prefer this call-site over
// mov-rdi prep tourism and over `0xa2caf0` body. Prefer over deepen
// `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
// `0x14c2f0` / `0x6a5370`. Prefer over trampoline/`0x52eb50` /
// `0x26800`/`0xf590` / staffroll / vcall-callee / `0x53fd10` /
// `0x17070` body tourism.
//
// Claimed: call-site control flow pin only.
// NOT modelled: mov rdi,[r13] prep; 0xa2caf0 body; following second
// mov rdi,[r13] / stack prep. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseA2caf0Rva = 0x5323f0;
inline constexpr std::size_t kInit531d60PostBaseA2caf0Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseA2caf0EndRva = 0x5323f5;
inline constexpr std::uint64_t kInit531d60PostBaseA2caf0TargetRva = 0xa2caf0;
inline constexpr std::uint64_t kInit531d60PostBaseA2caf0PrepRva = 0x5323ec;
inline constexpr std::size_t kInit531d60PostBaseA2caf0PrepSize = 4;

struct Init531d60PostBaseA2caf0Hooks {
    // Models call 0xa2caf0 (callee body NOT lifted). Null → no-op.
    void (*call_a2caf0)() = nullptr;
};

struct Init531d60PostBaseA2caf0Result {
    bool ok = false;
    int call_a2caf0_count = 0;
};

// Models OBS 0x5323f0..0x5323f5 on measured Init success path.
[[nodiscard]] Init531d60PostBaseA2caf0Result init_531d60_post_base_a2caf0(
    const Init531d60PostBaseA2caf0Hooks& hooks = {}) noexcept;

}  // namespace sotc
