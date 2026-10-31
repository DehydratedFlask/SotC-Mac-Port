// Init `0x531d60` continue call-site OBS `0x532731..0x532736` (5 B):
//   call 0xa2ea00   ; first unit after E509 second post-a2ed00-u7 0xf590
//   // stop before mov rdi,rbx @ 0x532736
//
// Always-on after E509 u14. Intervening prep `0x532728..0x532731` (9 B:
// mov rbx,[r13] / xor esi,esi / mov rdi,rbx) is NOT modelled — prefer this
// call-site over prep tourism and over `0xa2ea00` body. Prefer over deepen
// `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
// `0x14c2f0` / `0x6a5370`. Prefer over trampoline/`0x52eb50` / `0x26800` /
// staffroll / vcall-callee / `0xa2ed00` / `0xa2caf0` / `0xf590` body tourism.
//
// Distinct from grouped `continue_a` (E519) and from all E489..E509 f590/a2ed00
// single call-site modules.
// Claimed: call-site control flow pin only.
// NOT modelled: 9 B prep; 0xa2ea00 body; following mov rdi,rbx.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseA2ea00U1Rva = 0x532731;
inline constexpr std::size_t kInit531d60PostBaseA2ea00U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseA2ea00U1EndRva = 0x532736;
inline constexpr std::uint64_t kInit531d60PostBaseA2ea00U1TargetRva = 0xa2ea00;
inline constexpr std::uint64_t kInit531d60PostBaseA2ea00U1PrepRva = 0x532728;
inline constexpr std::size_t kInit531d60PostBaseA2ea00U1PrepSize = 9;

struct Init531d60PostBaseA2ea00U1Hooks {
    // Optional stand-in for rbx after the demoted [r13] load; NOT required for ok.
    const void* incoming_rbx = nullptr;
    // 0xa2ea00(rdi=rbx) at 0x532731 (body NOT lifted). Null => no-op.
    int (*call_a2ea00)(void* rbx) = nullptr;
};

struct Init531d60PostBaseA2ea00U1Result {
    bool ok = false;
    int call_a2ea00_count = 0;
    int eax_return = 0;
};

[[nodiscard]] Init531d60PostBaseA2ea00U1Result init_531d60_post_base_a2ea00_u1(
    const Init531d60PostBaseA2ea00U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
