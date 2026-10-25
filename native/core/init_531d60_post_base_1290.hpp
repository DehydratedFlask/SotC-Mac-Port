// Init `0x531d60` continue call-site OBS `0x531e03..0x531e08` (5 B):
//   call 0x1290
//   // stop before next call 0x56db70 @ 0x531e08
//
// Always-on after E448 0x5c64c0 call-site. Prefer over full 0x1290 body
// (~190 B; guard cell 0x15fe518; nested 0x1bde30 + vcall helpers) and over
// deepen 0x5c64c0 / 0x56dcb0 / 0x499f80 / 0x140d90 / 0x14c2f0 / 0x6a5370.
// Prefer over empty-path migrate/non-null.
//
// Claimed: call-site control flow pin only.
// NOT modelled: body of 0x1290. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase1290Rva = 0x531e03;
inline constexpr std::size_t kInit531d60PostBase1290Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase1290EndRva = 0x531e08;
inline constexpr std::uint64_t kInit531d60PostBase1290TargetRva = 0x1290;

struct Init531d60PostBase1290Hooks {
    // Models 0x1290(). Null → no-op.
    void (*call_1290)() = nullptr;
};

struct Init531d60PostBase1290Result {
    bool ok = false;
    int call_1290_count = 0;
};

// Models OBS 0x531e03..0x531e08 on measured Init success path.
[[nodiscard]] Init531d60PostBase1290Result init_531d60_post_base_1290(
    const Init531d60PostBase1290Hooks& hooks = {}) noexcept;

}  // namespace sotc
