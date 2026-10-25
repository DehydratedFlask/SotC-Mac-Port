// Init `0x531d60` continue call-site OBS `0x531e46..0x531e4b` (5 B):
//   call 0xf90310
//   // stop before next vmovups ymm @ 0x531e4b
//
// Always-on after E455 0x56fad0 call-site. Target 0xf90310 is the splash-hide
// PLT thunk (jmp [GOT]; Sony sceSystemServiceHideSplashScreen) — claim the
// call-site, not the PLT/Sony body. Prefer over staffroll body 0x7f7220 and
// over deepen 0x1290 / 0x5c64c0 / 0x56dcb0 / 0x499f80 / 0x140d90 / 0x14c2f0 /
// 0x6a5370. Prefer over empty-path migrate/non-null.
//
// Claimed: call-site control flow pin only.
// NOT modelled: PLT/Sony body / later ymm Init continues. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseF90310Rva = 0x531e46;
inline constexpr std::size_t kInit531d60PostBaseF90310Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseF90310EndRva = 0x531e4b;
inline constexpr std::uint64_t kInit531d60PostBaseF90310TargetRva = 0xf90310;

struct Init531d60PostBaseF90310Hooks {
    // Models 0xf90310() (splash-hide PLT; native no-op contract). Null → no-op.
    void (*call_f90310)() = nullptr;
};

struct Init531d60PostBaseF90310Result {
    bool ok = false;
    int call_f90310_count = 0;
};

// Models OBS 0x531e46..0x531e4b on measured Init success path.
[[nodiscard]] Init531d60PostBaseF90310Result init_531d60_post_base_f90310(
    const Init531d60PostBaseF90310Hooks& hooks = {}) noexcept;

}  // namespace sotc
