// Init `0x531d60` post-base continue call-site OBS `0x531ddf..0x531df4` (21 B):
//   test al, al; jz fail
//   lea rdi, [r12+0x3c70]; call 0x499f80
//   // stop before next call 0x56dcb0 @ 0x531df4
//
// Always-on after successful Base Initialize return (which includes completed
// measured 0x533100 body + post-vslot 0x357f0). Prefer over full 0x499f80 body
// (193 B; alloc/ctor/global replace) and over deepen 0x140d90 / 0x14c2f0 /
// 0x6a5370 (helper tourism). Prefer over empty-path migrate/non-null.
//
// Claimed: call-site control flow + arg pin (rdi = self+0x3c70).
// NOT modelled: body of 0x499f80; 0x56dcb0 / 0x5c64c0 / subsequent Init leaves
// already covered by E406 remainder. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase499f80Rva = 0x531ddf;
inline constexpr std::size_t kInit531d60PostBase499f80Size = 21;
inline constexpr std::uint64_t kInit531d60PostBase499f80EndRva = 0x531df4;
inline constexpr std::uint64_t kInit531d60PostBase499f80TargetRva = 0x499f80;
inline constexpr std::size_t kInit531d60PostBase499f80ArgOff = 0x3c70;

struct Init531d60PostBase499f80Hooks {
    // Models 0x499f80(arg). Null → no-op.
    void (*call_499f80)(void* arg) = nullptr;
};

struct Init531d60PostBase499f80Result {
    bool ok = false;
    void* arg = nullptr;
    int call_499f80_count = 0;
};

// Models OBS 0x531ddf..0x531df4 on success path (base returned true / al!=0).
[[nodiscard]] Init531d60PostBase499f80Result init_531d60_post_base_499f80(
    void* self, const Init531d60PostBase499f80Hooks& hooks = {}) noexcept;

}  // namespace sotc
