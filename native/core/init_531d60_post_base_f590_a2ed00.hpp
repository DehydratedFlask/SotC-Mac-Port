// Init `0x531d60` continue call-site OBS `0x532465..0x53246a` (5 B):
//   call 0xf590   ; first unit after E489 first 0xa2ed00 call-site
//   // stop before mov rdi,r15 / call 0xf590 @ 0x53246a
//
// Always-on after E489 first 0xa2ed00. Intervening prep `0x532462..0x532465`
// (3 B: mov rdi,rbx) is NOT modelled — prefer this call-site over mov-rdi
// prep tourism and over `0xf590` body. Prefer over deepen `0x1290` /
// `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
// `0x6a5370`. Prefer over trampoline/`0x52eb50` / `0x26800` / staffroll /
// vcall-callee / `0xa2ed00` / `0xa2caf0` body tourism.
//
// Distinct from earlier dual call-site `26800_f590` (E461).
// Claimed: call-site control flow pin only.
// NOT modelled: mov rdi,rbx prep; 0xf590 body; following second call 0xf590
// (call-site claimed separately as E491; body still demoted). PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00Rva = 0x532465;
inline constexpr std::size_t kInit531d60PostBaseF590A2ed00Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00EndRva = 0x53246a;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00TargetRva = 0xf590;
inline constexpr std::uint64_t kInit531d60PostBaseF590A2ed00PrepRva = 0x532462;
inline constexpr std::size_t kInit531d60PostBaseF590A2ed00PrepSize = 3;

struct Init531d60PostBaseF590A2ed00Hooks {
    // Models call 0xf590 (callee body NOT lifted). Null → no-op.
    void (*call_f590)() = nullptr;
};

struct Init531d60PostBaseF590A2ed00Result {
    bool ok = false;
    int call_f590_count = 0;
};

// Models OBS 0x532465..0x53246a on measured Init success path.
[[nodiscard]] Init531d60PostBaseF590A2ed00Result init_531d60_post_base_f590_a2ed00(
    const Init531d60PostBaseF590A2ed00Hooks& hooks = {}) noexcept;

}  // namespace sotc
