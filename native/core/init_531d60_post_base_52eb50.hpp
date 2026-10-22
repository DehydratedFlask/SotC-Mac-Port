// Init `0x531d60` continue call-site OBS `0x532031..0x532036` (5 B):
//   call 0x52eb50
//   // stop before lea rbx @ 0x532036
//
// Always-on after E463 Shadow Boss Outro Jump pair. Target 0x52eb50 is a
// trampoline (lea/lea/mov/jmp → 0xa44da0) — claim the **call-site**, not the
// body. Prefer over deepen 0x1290 / 0x5c64c0 / 0x56dcb0 / 0x499f80 /
// 0x140d90 / 0x14c2f0 / 0x6a5370. Prefer over 0x26800/0xf590 / staffroll/PLT
// body tourism.
//
// Claimed: call-site control flow pin only.
// NOT modelled: 0x52eb50 body / later Init continues. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase52eb50Rva = 0x532031;
inline constexpr std::size_t kInit531d60PostBase52eb50Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase52eb50EndRva = 0x532036;
inline constexpr std::uint64_t kInit531d60PostBase52eb50TargetRva = 0x52eb50;

struct Init531d60PostBase52eb50Hooks {
    // Models 0x52eb50() (trampoline body NOT lifted). Null → no-op.
    void (*call_52eb50)() = nullptr;
};

struct Init531d60PostBase52eb50Result {
    bool ok = false;
    int call_52eb50_count = 0;
};

// Models OBS 0x532031..0x532036 on measured Init success path.
[[nodiscard]] Init531d60PostBase52eb50Result init_531d60_post_base_52eb50(
    const Init531d60PostBase52eb50Hooks& hooks = {}) noexcept;

}  // namespace sotc
