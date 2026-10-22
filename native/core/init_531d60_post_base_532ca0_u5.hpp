// Init `0x531d60` continue call-site OBS `0x532272..0x532277` (5 B):
//   call 0x532ca0   ; fifth unit after E473 fourth virtual-call +0x18
//   // stop before movq/virtual-call path @ 0x532277
//
// Always-on after E473 vcall18_u4. Intervening stack prep
// `0x532212..0x532272` (96 B) is NOT modelled — prefer this call-site over
// stack-prep tourism and over `0x532ca0` body (`push rbp` helper). Prefer over
// deepen `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` /
// `0x14c2f0` / `0x6a5370`. Prefer over trampoline/`0x52eb50` / `0x26800`/
// `0xf590` / staffroll / vcall-callee body tourism.
//
// Claimed: fifth-unit call-site control flow pin only.
// NOT modelled: stack prep; 0x532ca0 body; following virtual call
// (claimed separately as E475). PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase532ca0U5Rva = 0x532272;
inline constexpr std::size_t kInit531d60PostBase532ca0U5Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase532ca0U5EndRva = 0x532277;
inline constexpr std::uint64_t kInit531d60PostBase532ca0U5TargetRva = 0x532ca0;
inline constexpr std::uint64_t kInit531d60PostBase532ca0U5StackPrepRva = 0x532212;
inline constexpr std::size_t kInit531d60PostBase532ca0U5StackPrepSize = 96;

struct Init531d60PostBase532ca0U5Hooks {
    // Models call 0x532ca0 (callee body NOT lifted). Null → no-op.
    void (*call_532ca0)() = nullptr;
};

struct Init531d60PostBase532ca0U5Result {
    bool ok = false;
    int call_532ca0_count = 0;
};

// Models OBS 0x532272..0x532277 on measured Init success path.
[[nodiscard]] Init531d60PostBase532ca0U5Result init_531d60_post_base_532ca0_u5(
    const Init531d60PostBase532ca0U5Hooks& hooks = {}) noexcept;

}  // namespace sotc
