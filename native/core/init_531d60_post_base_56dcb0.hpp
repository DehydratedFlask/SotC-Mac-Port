// Init `0x531d60` continue call-site OBS `0x531df4..0x531df9` (5 B):
//   call 0x56dcb0
//   // stop before next call 0x56dca0 @ 0x531df9 (already a ret-nop in E406 remainder)
//
// Always-on after E446 post-base 0x499f80 call-site. Prefer over full 0x56dcb0
// body (126 B; stack-chk cell 0x15fe518; nested 0xa5cd90/0x946880/0xa5d990/
// 0x560b60) and over deepen 0x499f80 / 0x140d90 / 0x14c2f0 / 0x6a5370.
// Prefer over 0x5c64c0 (later Init continue). Prefer over empty-path
// migrate/non-null.
//
// Claimed: call-site control flow pin only.
// NOT modelled: body of 0x56dcb0. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase56dcb0Rva = 0x531df4;
inline constexpr std::size_t kInit531d60PostBase56dcb0Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase56dcb0EndRva = 0x531df9;
inline constexpr std::uint64_t kInit531d60PostBase56dcb0TargetRva = 0x56dcb0;

struct Init531d60PostBase56dcb0Hooks {
    // Models 0x56dcb0(). Null → no-op.
    void (*call_56dcb0)() = nullptr;
};

struct Init531d60PostBase56dcb0Result {
    bool ok = false;
    int call_56dcb0_count = 0;
};

// Models OBS 0x531df4..0x531df9 on measured Init success path.
[[nodiscard]] Init531d60PostBase56dcb0Result init_531d60_post_base_56dcb0(
    const Init531d60PostBase56dcb0Hooks& hooks = {}) noexcept;

}  // namespace sotc
