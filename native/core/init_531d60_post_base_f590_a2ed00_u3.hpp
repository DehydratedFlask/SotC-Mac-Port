// Init `0x531d60` continue call-site OBS `0x5324db..0x5324e0` (5 B):
//   call 0xf590   ; first unit after E492 second 0xa2ed00 call-site
//   // stop before mov rdi,r15 / call 0xf590 @ 0x5324e0
//
// Always-on after E492 second 0xa2ed00. Intervening prep `0x5324d8..0x5324db`
// (3 B: mov rdi,rbx) is NOT modelled — prefer this call-site over mov-rdi
// prep tourism and over `0xf590` body. Prefer over deepen `0x1290` /
// `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
// `0x6a5370`. Prefer over trampoline/`0x52eb50` / `0x26800` / staffroll /
// vcall-callee / `0xa2ed00` / `0xa2caf0` body tourism.
//
// Distinct from earlier dual call-site `26800_f590`, first post-a2ed00
// `f590_a2ed00` (E490), and second post-a2ed00 `f590_a2ed00_u2` (E491).
// Claimed: call-site control flow pin only.
// NOT modelled: mov rdi,rbx prep; 0xf590 body; following second call 0xf590.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U3Rva = 0x5324db;
inline constexpr std::size_t kInit531d60PostBaseF590A2ed00U3Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U3EndRva = 0x5324e0;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U3TargetRva = 0xf590;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00U3PrepRva = 0x5324d8;
inline constexpr std::size_t kInit531d60PostBaseF590A2ed00U3PrepSize = 3;

struct Init531d60PostBaseF590A2ed00U3Hooks {
    // Models call 0xf590 (callee body NOT lifted). Null → no-op.
    void (*call_f590)() = nullptr;
};

struct Init531d60PostBaseF590A2ed00U3Result {
    bool ok = false;
    int call_f590_count = 0;
};

// Models OBS 0x5324db..0x5324e0 on measured Init success path.
[[nodiscard]] Init531d60PostBaseF590A2ed00U3Result init_531d60_post_base_f590_a2ed00_u3(
    const Init531d60PostBaseF590A2ed00U3Hooks& hooks = {}) noexcept;

}  // namespace sotc
