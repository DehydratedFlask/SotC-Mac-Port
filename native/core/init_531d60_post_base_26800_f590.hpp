// Init `0x531d60` continue call-site OBS `0x531f71..0x531f93` (34 B):
//   vxorps / lea rdi / vmovups xmm [rsp+…]   ; stack prep (NOT modelled)
//   call 0x26800
//   mov rdi, rbx
//   call 0xf590
//   // stop before next string setup @ 0x531f93 ("Shadow Boss Jump")
//
// Always-on after E460 ymm reload. Targets 0x26800 / 0xf590 are string-path
// helpers (push rbp bodies) — claim the **call-sites**, not the bodies.
// Prefer over deepen 0x1290 / 0x5c64c0 / 0x56dcb0 / 0x499f80 / 0x140d90 /
// 0x14c2f0 / 0x6a5370. Prefer over staffroll 0x7f7220 body / empty-path
// migrate/non-null.
//
// Claimed: dual call-site control flow pin only.
// NOT modelled: stack prep semantics; 0x26800 / 0xf590 bodies; later string
// pairs. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase26800F590Rva = 0x531f71;
inline constexpr std::size_t kInit531d60PostBase26800F590Size = 34;
inline constexpr std::uint64_t kInit531d60PostBase26800F590EndRva = 0x531f93;
inline constexpr std::uint64_t kInit531d60PostBase26800F590Call26800Rva = 0x531f86;
inline constexpr std::uint64_t kInit531d60PostBase26800F590CallF590Rva = 0x531f8e;
inline constexpr std::uint64_t kInit531d60PostBase26800TargetRva = 0x26800;
inline constexpr std::uint64_t kInit531d60PostBaseF590TargetRva = 0xf590;

struct Init531d60PostBase26800F590Hooks {
    // Models 0x26800() (string helper body NOT lifted). Null → no-op.
    void (*call_26800)() = nullptr;
    // Models 0xf590() (path/dtor helper body NOT lifted). Null → no-op.
    void (*call_f590)() = nullptr;
};

struct Init531d60PostBase26800F590Result {
    bool ok = false;
    int call_26800_count = 0;
    int call_f590_count = 0;
};

// Models OBS 0x531f71..0x531f93 on measured Init success path.
[[nodiscard]] Init531d60PostBase26800F590Result init_531d60_post_base_26800_f590(
    const Init531d60PostBase26800F590Hooks& hooks = {}) noexcept;

}  // namespace sotc
