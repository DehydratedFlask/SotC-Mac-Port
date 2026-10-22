// Init `0x531d60` continue call-site OBS `0x531dfe..0x531e03` (5 B):
//   call 0x5c64c0
//   // stop before next call 0x1290 @ 0x531e03
//
// Always-on after E447 0x56dcb0 call-site. Intervening 0x56dca0 @ 0x531df9 is a
// one-byte `ret` already modelled as E406 ret-nop — not re-claimed here.
// Prefer over full 0x5c64c0 body (large AVX BSS init + nested helpers; jmp
// epilogue) and over deepen 0x56dcb0 / 0x499f80 / 0x140d90 / 0x14c2f0 /
// 0x6a5370. Prefer over empty-path migrate/non-null.
//
// Claimed: call-site control flow pin only.
// NOT modelled: body of 0x5c64c0. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase5c64c0Rva = 0x531dfe;
inline constexpr std::size_t kInit531d60PostBase5c64c0Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase5c64c0EndRva = 0x531e03;
inline constexpr std::uint64_t kInit531d60PostBase5c64c0TargetRva = 0x5c64c0;

struct Init531d60PostBase5c64c0Hooks {
    // Models 0x5c64c0(). Null → no-op.
    void (*call_5c64c0)() = nullptr;
};

struct Init531d60PostBase5c64c0Result {
    bool ok = false;
    int call_5c64c0_count = 0;
};

// Models OBS 0x531dfe..0x531e03 on measured Init success path.
[[nodiscard]] Init531d60PostBase5c64c0Result init_531d60_post_base_5c64c0(
    const Init531d60PostBase5c64c0Hooks& hooks = {}) noexcept;

}  // namespace sotc
